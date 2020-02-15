/*Example sketch to control a stepper motor with A4988 stepper motor driver, AccelStepper library and Arduino: continuous rotation. More info: https://www.makerguides.com */
// Include the AccelStepper library:
#include <AccelStepper.h>
#include <EEPROM.h>

// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define motorInterfaceType 1

// Available Digital (not PWM or Int) 4,5,8,11,12,13LED)
const int _buttonPins[] = {A0, A1, A2, A3 };
const int _gearMotorDriverPins[] = {3,4,7,8}; //3=PWM 4=STBY, 7=AIN1, 8=AIN2
//  const int _stepperMotorDriverPins[] = {4,7,8,12,13}; // stepPin, dirPin, ms1Pin, ms2Pin, enablePin


const int _limitSwitchPin = 2; // Needs interupt 
const int _encoderPins [] = {1,11};  // 1 needs interupt
const int _timerLightsPins[] = {5, 6, 9, 10};  // Needs PWM (3,5,6,9, or 10)



#define MOVING true
#define STOPPED false
#define OPENING true
#define CLOSING false
#define DOORCLOSED -1
#define DOOROPEN +1
#define DOORPARTIAL 0
#define MAGICNUMBER 69


unsigned long _debounceDelay = 500;    // the debounce time; increase if the bouncy
unsigned long _currentCountdown = 0; // miliseconds.  Will rollover after 49.71 days
byte _timeLights = 0;

bool _motorState = STOPPED;
bool _direction = OPENING;
bool _adjusting = false;
int _doorState = DOORCLOSED;
int _currentPosition = 0;
int _maxPosition = 720;

bool _timerRunning = true;
int period = 500;

int _eeAddress = 0;
struct ConfigDataStruct {
  int maxPosition;
  int initialized;
};

ConfigDataStruct _configData;

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, _stepperMotorDriverPins[0], _stepperMotorDriverPins[1]);
void setup() 
{
    // initialize the pushbutton pins as an input:
    initializeButtons();

    // Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:

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
    checkTimer();
    updateMotor();
    simulateMotor();
    updateLEDs();
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


int getPosition()
{
    return _currentPosition;
}

void checkTimer()
{
    
}

void updateLEDs()
{
    static int pulsePeriod = 2000;
    
    int pwmVal = 255;
    int mask = 1;
    if(_timerRunning)
    {
        pwmVal = int(255.0 * (1.0 + cos(2.0 * PI * millis() / 1000.0)) / 2.0);
    }
    for (int i=0; i <= 3; i++)
    {
        if(_timeLights & (mask << i))
        {            
            analogWrite(_timerLightsPins[i],pwmVal);            
        } 
        else
        {
            analogWrite(_timerLightsPins[i], 0);
        }
    }
}
