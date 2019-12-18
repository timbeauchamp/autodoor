    // initialize the pushbutton pin as an input:

void initializeButtons(bool withInterupt)
{
    pinMode(_buttonPins[0], INPUT_PULLUP);
    pinMode(_buttonPins[1], INPUT_PULLUP);
    pinMode(_buttonPins[2], INPUT_PULLUP);
    pinMode(_buttonPins[3], INPUT_PULLUP);

    if(withInterupt)
    {
        attachInterrupt(digitalPinToInterrupt(_buttonPins[0]), pressedInterupt, FALLING);
        attachInterrupt(digitalPinToInterrupt(_buttonPins[1]), pressedInterupt, FALLING);
        attachInterrupt(digitalPinToInterrupt(_buttonPins[2]), pressedInterupt, FALLING);
        attachInterrupt(digitalPinToInterrupt(_buttonPins[3]), pressedInterupt, FALLING);
    }
}

void pressedInterupt()
{  
    static unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled 
    
    int reading1 = digitalRead(_buttonPins[0]);
    int reading2 = digitalRead(_buttonPins[1]);
    int reading3 = digitalRead(_buttonPins[2]);
    int reading4 = digitalRead(_buttonPins[3]);

    if ((millis() - lastDebounceTime) > _debounceDelay) 
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

void checkButtons()  //  Non interupt version
{  
    static bool buttonStates[] = {HIGH, HIGH, HIGH, HIGH};
    static unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled  
    
    int reading1 = digitalRead(_buttonPins[0]);
    int reading2 = digitalRead(_buttonPins[1]);
    int reading3 = digitalRead(_buttonPins[2]);
    int reading4 = digitalRead(_buttonPins[3]);

    if ((millis() - lastDebounceTime) > _debounceDelay) 
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
    // increment timer
    _timeLights++;
    _timeLights  = _timeLights & B00001111;
    showTimerLEDs();
}
