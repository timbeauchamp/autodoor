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