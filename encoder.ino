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
