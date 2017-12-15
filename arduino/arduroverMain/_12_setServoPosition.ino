// Function to set servo position
void setServoPosition(int pin, int angle) {
  // Attach requested servo
  servo.attach(pin);
  
  // Set servo position
  servo.write(angle);

  // Wait for servo to reach the position
  delay(500); // TODO: dynamic delay length

  // Detach the servo to stop jittering
  servo.detach();
}

