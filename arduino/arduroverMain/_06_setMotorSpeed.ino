// Function to set speed of one or both motors
byte setMotorSpeed(byte motor, byte pwm, byte dir) {
  switch(motor) {

    // Left side only
    case(MOTOR_LEFT): {
      // Set motor direction
      digitalWrite(MOTOR_LEFT_INA, dir);
      digitalWrite(MOTOR_LEFT_INB, dir^1);
      // Set motor speed
      analogWrite(MOTOR_LEFT_PWM, pwm);
    } break;

    // Right side only
    case(MOTOR_RIGHT): {
      // Set motor direction
      digitalWrite(MOTOR_RIGHT_INA, dir^1);
      digitalWrite(MOTOR_RIGHT_INB, dir);
      // Set motor speed
      analogWrite(MOTOR_RIGHT_PWM, pwm);
    } break;

    // Both sides
    case(MOTOR_BOTH): {
      // Set motor direction
      digitalWrite(MOTOR_LEFT_INA, dir);
      digitalWrite(MOTOR_LEFT_INB, dir^1);
      digitalWrite(MOTOR_RIGHT_INA, dir^1);
      digitalWrite(MOTOR_RIGHT_INB, dir);
      // Set motor speed
      analogWrite(MOTOR_LEFT_PWM, pwm);
      analogWrite(MOTOR_RIGHT_PWM, pwm);
    } break;
  
  }

  // Operation successful
  return(0x00);
}

