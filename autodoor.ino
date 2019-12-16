/*Example sketch to control a stepper motor with A4988 stepper motor driver, AccelStepper library and Arduino: continuous rotation. More info: https://www.makerguides.com */
// Include the AccelStepper library:
#include <AccelStepper.h>
#include <EEPROM.h>

// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1


#define MOVING true
#define STOPPED false
#define OPENING true
#define CLOSING false
#define DOORCLOSED -1
#define DOOROPEN +1
#define DOORPARTIAL 0
#define MAGICNUMBER 69

// constants won't change. They're used here to set pin numbers:
const int button1Pin = 18;     // the number of the pushbutton pin
const int button2Pin = 19;     // the number of the pushbutton pin
const int button3Pin = 20;     // the number of the pushbutton pin
const int button4Pin = 21;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin


// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 500;    // the debounce time; increase if the output flickers


bool _motorState = STOPPED;
bool _direction = OPENING;
bool _adjusting = false;
int _doorState = DOORCLOSED;
int _currentPosition = 0;
int _maxPosition = 720;
int _minPosition = 0;

int period = 500;
unsigned long time_now = 0;

int eeAddress = 0;
struct ConfigDataStruct {
  int minPosition;
  int maxPosition;
  int initialized;
};

ConfigDataStruct _configData;

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
void setup() 
{
    // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(button1Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button1Pin), pressedInterupt, FALLING);

  pinMode(button2Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button2Pin), pressedInterupt, FALLING);

  pinMode(button3Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button3Pin), pressedInterupt, FALLING);

  pinMode(button4Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button4Pin), pressedInterupt, FALLING);

  Serial.begin(9600);
  while (! Serial); // Wait for Serial 
  Serial.println("Ready");

  // Set the maximum speed in steps per second:
  stepper.setMaxSpeed(1000);
  // Set the speed in steps per second:
  stepper.setSpeed(400);

  loadConfig();
}

void loop() 
{
  updateMotor();
  simulateMotor();
  updateLED();
}

void updateMotor()
{

  if(_direction)
  {
    stepper.setSpeed(400);
  }
  else
  {
    stepper.setSpeed(-400);
  }
  
  if(_motorState)
  {
    stepper.runSpeed();
  }
  else
  {
    stepper.stop();
  }
}

void loadConfig()
{
  EEPROM.get(eeAddress, _configData); 
  Serial.println("Read config from EEPROM: ");
  Serial.println(_configData.minPosition);
  Serial.println(_configData.maxPosition);
  Serial.println(_configData.initialized);

  if(_configData.initialized == MAGICNUMBER)
  {
    Serial.println("Initialized: true");
    _minPosition = _configData.minPosition;
    _configData.maxPosition = _maxPosition;
  }
  else
  {
    Serial.println("Initialized: false, saving defaults");
    saveConfig();
  }
}

void saveConfig()
{
    Serial.println("Initialized: false");
    _configData.minPosition = _minPosition;
    _configData.maxPosition = _maxPosition;
    _configData.initialized = MAGICNUMBER;
    EEPROM.put(eeAddress, _configData);     
}

void adjustMinPosition(int amount)
{
  _minPosition += amount;
  saveConfig();
}

void adjustMaxPosition(int amount)
{
  _maxPosition += amount;
  saveConfig();
}

void stopMotor()
{
  Serial.println("Stopping Motor");
  _motorState = STOPPED;
  showStatus();
}

void startMotor()
{
  _motorState = MOVING;
}

void startStopMotor()
{
  _motorState = !_motorState;
  if(_motorState == STOPPED)
  {
      reverseMotor();
  }
}

void reverseMotor()
{
  _direction = !_direction;
}

void openDoor()
{
  _direction = OPENING;
  showStatus();
  startMotor();
}

void closeDoor()
{
  _direction = CLOSING;
  showStatus();
  startMotor();
}

void setMaxOpen(int maxOpen)
{
  _maxPosition = maxOpen;
}

int getMaxOpen()
{
  return _maxPosition;
}

void setMaxClose(int maxClose)
{
  _minPosition = maxClose;
}

int getMaxClose()
{
  return _minPosition;
}

int getPosition()
{
  return _currentPosition;
}


void positionUpdated()
{
  
  if(_motorState)
  {
    if(_adjusting)
    {
      if(_doorState == DOOROPEN && _direction == CLOSING && _currentPosition <= _maxPosition)
      {
        Serial.println("New Top");
        stopMotor();
        _adjusting = false;
      }else if(_doorState == DOORCLOSED && _direction == OPENING && _currentPosition >= _minPosition)
      {
        Serial.println("New Bottom");
        stopMotor();     
        _adjusting = false;

      }
    }
    else
    {
      if(_direction == OPENING && _currentPosition >= _maxPosition)
      {
        Serial.println("Top");
        _doorState = DOOROPEN;
        stopMotor();
        reverseMotor();
      }else if(_direction == CLOSING && _currentPosition <= _minPosition)
      {
        Serial.println("Bottom");
        _doorState = DOORCLOSED;
        stopMotor();     
        reverseMotor();
      }
      else
      {
        _doorState = DOORPARTIAL;
      }
    }
  }
}

void pressedInterupt()
{
  
  int reading1 = digitalRead(button1Pin);
  int reading2 = digitalRead(button2Pin);
  int reading3 = digitalRead(button3Pin);
  int reading4 = digitalRead(button4Pin);


  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    lastDebounceTime = millis();

    if(reading1 == LOW)
    {
      button1Pressed();
    }

    if(reading2 == LOW)
    {
      button2Pressed();
    }
    
    if(reading3 == LOW)
    {
      button3Pressed();
    }

    if(reading4 == LOW)
    {
      button4Pressed();
    }
    
  }else{
//    Serial.println("bounce");
  }
}
void  button1Pressed()
{
  Serial.println("Button 1 Pressed");
  startStopMotor();
}

void  button2Pressed()
{
  Serial.println("Button 2 Pressed, Adjusting Up");
  
  if( _motorState==STOPPED)
  {
    if(_doorState == DOOROPEN)
    {
      adjustMaxPosition(10);
      openDoor();
    }
    else if(_doorState == DOORCLOSED)
    {
      adjustMinPosition(10);
      _adjusting = true;
      startMotor();
    }
  }
}

void  button3Pressed()
{
  Serial.println("Button 3 Pressed, Adjusting down");
  
  if( _motorState==STOPPED)
  {
    if(_doorState == DOOROPEN)
    {
      adjustMaxPosition(-10);
      _adjusting = true;
      startMotor();
    }
    else if(_doorState == DOORCLOSED)
    {
      adjustMinPosition(-10);
      closeDoor();
    }
  }
}

void  button4Pressed()
{
  Serial.println("Button 4 Pressed");
 
}

void simulateMotor()
{
  static int delay = 7;
  static unsigned long lastMotorTick = 0;

  if(millis() - delay > lastMotorTick)
  {
    lastMotorTick = millis();
    
    if(_motorState)
    {
      if(_direction == OPENING)
      {
        _currentPosition += 1;
      }else
      {
        _currentPosition += -1;
      }
      
      if(millis() > time_now + period)
      {
        time_now = millis();  
        showStatus();
      } 
    }
    positionUpdated();
  }
}

void updateLED()
{
    // Update LED
    if(_motorState)
    {
      digitalWrite(ledPin, HIGH);
    }
    else
    {
      digitalWrite(ledPin, LOW);
    }
}

void showStatus()
{
     Serial.println("MotorState: " + trueFalse(_motorState));
     Serial.println("Direction: " + openingClosing(_direction));
     Serial.println("State: " + openClosed(_doorState));
     String position = "Position/min/max: ";
     position += _currentPosition;
     position += "/";
     position += _minPosition;
     position += "/";
     position += _maxPosition;
     position += "\nStepperFeedback: ";
     position += stepper.currentPosition();
     position += "\n\n";
     Serial.println(position);
}

String trueFalse(bool val)  // helper to give nice text
{
  if(val)
  {
    return "TRUE"; 
  }
  else
  {
    return "FALSE";     
  }
}

String openingClosing(bool val)  // helper to give nice text
{
  if(val == OPENING)
  {
    return "Opening"; 
  }
  else
  {
    return "Closing";     
  }
}

String openClosed(int val)  // helper to give nice text
{
  if(val == DOOROPEN)
  {
    return "Door Open"; 
  }
  else if(val == DOORCLOSED)
  {
    return "Door Closed";     
  }
  else
  {
    return "Door Partially Open/Closed";
  }
}
