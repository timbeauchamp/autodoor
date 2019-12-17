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
    // increment timer
    _timeLights++;
    _timeLights  = _timeLights & B00001111;
}
