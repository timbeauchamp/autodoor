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

const int _buttonPins[] = {18, 19, 20, 21};
const int _timerLightsPins[] =  {4, 5, 6, 7};
const int _ledPin =  13;      // the number of the LED pin


unsigned long _debounceDelay = 500;    // the debounce time; increase if the bouncy
unsigned long _currentCountdown = 0;
byte _timeLights = 0;

bool _motorState = STOPPED;
bool _direction = OPENING;
bool _adjusting = false;
int _doorState = DOORCLOSED;
int _currentPosition = 0;
int _maxPosition = 720;
int _minPosition = 0;

bool _timerRunning = true;
int period = 500;

int _eeAddress = 0;
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
    pinMode(_ledPin, OUTPUT);

    // initialize the pushbutton pins as an input:
    bool withInterupt = false;
    initializeButtons(withInterupt);

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
    checkButtons();
    updateMotor();
    simulateMotor();
    updateLEDs();
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



void updateLEDs()
{
    static int pulsePeriod = 2000;
    // Update LED
    if(_motorState)
    {
        digitalWrite(_ledPin, HIGH);
    }
    else
    {
        digitalWrite(_ledPin, LOW);
    }

    
    int pwmVal = 255;
    if(_timerRunning)
    {
        int pwmVal = int(255.0 * (1.0 + cos(2.0 * PI * millis() / 1000.0)) / 2.0);
    }

    for (int i=0; i <= 3; i++)
    {
        if(_timeLights & (2 ^ i))
        {
            analogWrite(_timerLightsPins[i],pwmVal);            
        } 
        else
        {
            analogWrite(_timerLightsPins[i], 0);
        }
    }
}
