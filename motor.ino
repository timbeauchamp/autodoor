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
