void loop() {
  Serial.println(errFlags, BIN);

  // Check if SD card is available
  if(!(errFlags & (1 << ERR_SD_NOT_FOUND))) {
    // Automatically measure all sensors
    measureSensorValues(0xFF);
    // Log sensor measurements to SD card
    logToSD();
  }

  // Wait for new LoRa packet
  byte state = lora.receive(in);

  if(state == ERR_NONE) {
    // If packet reception was successfull, get the command type
    byte cmdIn = in.data[0];
    switch(cmdIn) {

      // Stop both motors
      case(CMD_MOTOR_LR_STOP): {
        // Stop both motors and copy the result to response packet
        out.data[0] = cmdIn | stopMotors();
      } break;

      // Set speed of both motors
      case(CMD_MOTOR_LR_SPEED): {
        // Get the PWM value from command packet
        int pwm = in.data[1] | (in.data[2] << 8);
        // Set speed of both motors and copy the result to response packet
        out.data[0] = cmdIn | setMotorSpeed(MOTOR_BOTH, pwm, in.data[4]);
      } break;

      // Set left side speed
      case(CMD_MOTOR_L_SPEED): {
        // Get the PWM value from command packet
        int pwm = in.data[1] | (in.data[2] << 8);
        // Set left side speed and copy the result to response packet
        out.data[0] = cmdIn | setMotorSpeed(MOTOR_LEFT, pwm, in.data[4]);
      } break;

      // Get left side current
      case(CMD_MOTOR_L_CURRENT): {
        // Measure current of left side motors
        int current = analogRead(MOTOR_LEFT_CS);
        // Copy the value to response packet
        out.data[1] = (current >> 8) & 0xFF;
        out.data[2] = current & 0xFF;
        // Copy the operation result to response packet
        out.data[0] = cmdIn | 0x00;
      } break;

      // Set right side speed
      case(CMD_MOTOR_R_SPEED): {
        // Get the PWM value from command packet
        int pwm = in.data[1] | (in.data[2] << 8);
        // Set right side speed and copy the result to response packet
        out.data[0] = cmdIn | setMotorSpeed(MOTOR_RIGHT, pwm, in.data[4]);
      } break;

      // Get right side current
      case(CMD_MOTOR_R_CURRENT): {
        // Measure current of left side motors
        int current = analogRead(MOTOR_RIGHT_CS);
        // Copy the value to response packet
        out.data[1] = (current >> 8) & 0xFF;
        out.data[2] = current & 0xFF;
        // Copy the operation result to response packet
        out.data[0] = cmdIn | 0x00;
      } break;

      // Tilt camera
      case(CMD_CAMERA_TILT): {
        // Set tilt servo position
        setServoPosition(CAMERA_TILT, in.data[1]);
        // Copy the operation result to response packet
        out.data[0] = cmdIn | 0x00;
      } break;

      // Pan camera
      case(CMD_CAMERA_PAN): {
        // Set pan servo position
        setServoPosition(CAMERA_PAN, in.data[1]);
        // Copy the operation result to response packet
        out.data[0] = cmdIn | 0x00;
      } break;

      // Take a picture with the camera
      case(CMD_CAMERA_SNAP): {
        // Try to take a picture and copy the operation result to response packet
        out.data[0] = cmdIn | takePicture();
      } break;

      // Send a picture stored on SD card
      case(CMD_CAMERA_SEND): {
        // Try to send the image and copy the operation result to response packet
        out.data[0] = cmdIn | sendPicture(in.data[1]);
      } break;

      // Force new sensor measurement
      case(CMD_SENSOR_MEASURE): {
        // Measure the specified sensors
        out.data[0] = cmdIn | measureSensorValues(in.data[1]);
      } break;

      // Send latest sensor data
      case(CMD_SENSOR_SEND): {
        // Send the latest data of specified sensors
        out.data[0] = cmdIn | sendSensorValues(in.data[1]);
      } break;

      // Resend the last LoRa packet
      case(CMD_LORA_RESEND_LAST): {
        // Transmit the last packet
        lora.transmit(last);
        // Copy the operation result to response packet
        out.data[0] = cmdIn | 0x00;
      } break;

      // Reconfigure LoRa module
      case(CMD_LORA_CONFIG): {
        out.data[0] = cmdIn;
        // Set new bandwidth, spreading factor and coding rate
        if(lora.setBandwidth(in.data[1]) == ERR_INVALID_BANDWIDTH) {
          // Specified bandwidth value is invalid
          out.data[0] |= 0x01;
        } else if(lora.setSpreadingFactor(in.data[2]) == ERR_INVALID_SPREADING_FACTOR) {
          // Specified spreading factor value is invalid
          out.data[0] |= 0x02;
        } else if(lora.setCodingRate(in.data[3]) == ERR_INVALID_CODING_RATE) {
          // Specified coding rate value is invalid
          out.data[0] |= 0x04;
        } else {
          // LoRa modem reconfiguration successful
          out.data[0] |= 0x00;
        }
      } break;

      // Send error flags
      case(CMD_LORA_REQUEST_ERRORS): {
        // Copy error flags to response packet
        out.data[1] = (errFlags >> 8) & 0xFF;
        out.data[2] = errFlags & 0xFF;
        // Copy the operation result to response packet
        out.data[0] = cmdIn | 0x00;
      } break;
    }

    // Transmit response packet
    lora.transmit(out);

    // Copy the response packet in case it needs to be resend
    out.copyInto(last);
    
  } else if(state == ERR_CRC_MISMATCH) {
    // If the received packet was damaged, request a resend
    out.data[0] = CMD_LORA_RESEND_LAST;
    lora.transmit(out);
  }
}

