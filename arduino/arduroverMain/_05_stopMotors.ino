// Functon to instantly stop all motors
byte stopMotors(void) {
  // Brake left side
  digitalWrite(MOTOR_LEFT_INA, HIGH);
  digitalWrite(MOTOR_LEFT_INB, HIGH);
  analogWrite(MOTOR_LEFT_PWM, 0);

  // Brake right side
  digitalWrite(MOTOR_RIGHT_INA, HIGH);
  digitalWrite(MOTOR_RIGHT_INB, HIGH);
  analogWrite(MOTOR_RIGHT_PWM, 0);

  // Operation successful
  return(0x00);
}

