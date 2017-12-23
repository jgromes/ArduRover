void loop() {
  //Serial.println(errFlags, BIN);

  // Check if SD card is available
  if(!(errFlags & (1 << ERR_SD_NOT_FOUND))) {
    // Check if camera is accessible
    if(errFlags & (1 << ERR_CAMERA_NOT_FOUND)) {
      // If not, try to access it again
      if(cam.begin()) {
        // If the camera was found, clear error bit
        errFlags &= ~(1 << ERR_CAMERA_NOT_FOUND);
        // Camera is a non-critical system, we can continue without it
      }
    }
    // Automatically measure all sensors
    measureSensorValues(0xFF);
    // Log sensor measurements to SD card
    logToSD();
  }

  // Wait for new LoRa packet
  byte state = lora.receive(in);

  if(state == ERR_NONE) {
    // Create a buffer to store the packet
    byte cmd[4] = {0, 0, 0, 0};

    // Empty output packet and convert input signed characters to unsigned bytes
    for(int i = 0; i < 4; i++) {
      out.data[i] = 0x00;
      cmd[i] = in.data[i] & 0xFF;
    }
    
    /*Serial.print("cmdIn:\t");
    for(int i = 0; i < 4; i++) {
      Serial.print(cmd[i], HEX);
      Serial.print('\t');
    }
    Serial.println();*/
    
    switch(cmd[0]) {

      // Stop both motors
      case(CMD_MOTOR_LR_STOP): {
        // Stop both motors and copy the result to response packet
        out.data[0] = cmd[0] | stopMotors();
      } break;

      // Set speed of both motors
      case(CMD_MOTOR_LR_SPEED): {
        // Set speed of both motors and copy the result to response packet
        out.data[0] = cmd[0] | setMotorSpeed(MOTOR_BOTH, cmd[1], cmd[2]);
      } break;

      // Set left side speed
      case(CMD_MOTOR_L_SPEED): {
        // Set left side speed and copy the result to response packet
        out.data[0] = cmd[0] | setMotorSpeed(MOTOR_LEFT, cmd[1], cmd[2]);
      } break;

      // Set right side speed
      case(CMD_MOTOR_R_SPEED): {
        // Set right side speed and copy the result to response packet
        out.data[0] = cmd[0] | setMotorSpeed(MOTOR_RIGHT, cmd[1], cmd[2]);
      } break;

      // Tilt camera
      case(CMD_CAMERA_TILT): {
        // Set tilt servo position
        setServoPosition(CAMERA_TILT, cmd[1]);
        // Copy the operation result to response packet
        out.data[0] = cmd[0] | 0x00;
      } break;

      // Pan camera
      case(CMD_CAMERA_PAN): {
        // Set pan servo position
        setServoPosition(CAMERA_PAN, cmd[1]);
        // Copy the operation result to response packet
        out.data[0] = cmd[0] | 0x00;
      } break;

      // Take a picture with the camera
      case(CMD_CAMERA_SNAP): {
        // Try to take a picture and copy the operation result to response packet
        out.data[0] = cmd[0] | takePicture();
      } break;

      // Send a picture stored on SD card
      case(CMD_CAMERA_SEND): {
        // Try to send the image and copy the operation result to response packet
        out.data[0] = cmd[0] | sendPicture(cmd[1]);
      } break;

      // Force new sensor measurement
      case(CMD_SENSOR_MEASURE): {
        // Measure the specified sensors
        out.data[0] = cmd[0] | measureSensorValues(cmd[1]);
      } break;

      // Send latest sensor data
      case(CMD_SENSOR_SEND): {
        // Send the latest data of specified sensors
        out.data[0] = cmd[0] | sendSensorValues(cmd[1]);
      } break;

      // Resend the last LoRa packet
      case(CMD_LORA_RESEND_LAST): {
        // Transmit the last packet
        lora.transmit(last);
        // Copy the operation result to response packet
        out.data[0] = cmd[0] | 0x00;
      } break;

      // Reconfigure LoRa module
      case(CMD_LORA_CONFIG): {
        out.data[0] = cmd[0];
        // Set new bandwidth, spreading factor and coding rate
        if(lora.setBandwidth(cmd[1]) == ERR_INVALID_BANDWIDTH) {
          // Specified bandwidth value is invalid
          out.data[0] |= 0x01;
        } else if(lora.setSpreadingFactor(cmd[2]) == ERR_INVALID_SPREADING_FACTOR) {
          // Specified spreading factor value is invalid
          out.data[0] |= 0x02;
        } else if(lora.setCodingRate(cmd[3]) == ERR_INVALID_CODING_RATE) {
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
        out.data[0] = cmd[0] | 0x00;
      } break;
    }

    /*Serial.print("cmdOut:\t");
    for(int i = 0; i < 4; i++) {
      Serial.print(out.data[i], HEX);
      Serial.print('\t');
    }
    Serial.println();*/

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

