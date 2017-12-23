// Function to measure values of specified sensor(s)
byte measureSensorValues(byte sen) {
  // Variable to store status of the current sensor
  byte state;

  // Response variable
  byte response = 0x00;

  // BM1422GMV measurement was requested
  if((sen >> 0) & 1) {
    // Measure sensor
    state = sensorMag.measure();
    // Copy the operation result into response variable
    response |= state << 0;
    // Update error flags
    setBit(&errFlags, ERR_SENSOR_BM1422GMV, state);
    if(state == 0) {
      // Measurement was successful, update measurement variables
      magneticX.number = sensorMag.magX;
      magneticY.number = sensorMag.magY;
      magneticZ.number = sensorMag.magZ;
    }

  // RPR-0521RS measurement was requested
  } else if((sen >> 1) & 1) {
    // Measure sensor
    state = sensorAls.measure();
    // Copy the operation result into response variable
    response |= state << 1;
    // Update error flags
    setBit(&errFlags, ERR_SENSOR_RPR_0521RS, state);
    if(state == 0) {
      // Measurement was successful, update measurement variables
      proximity.number = sensorAls.prox;
      ambientLight.number = sensorAls.ambLight;
    }

  // KX022-1020 measurement was requested
  } else if((sen >> 2) & 1) {
    // Measure sensor
    state = sensorAcc.measure();
    // Copy the operation result into response variable
    response |= state << 2;
    // Update error flags
    setBit(&errFlags, ERR_SENSOR_KX022_1020, state);
    if(state == 0) {
      // Measurement was successful, update measurement variables
      accelerationX.number = sensorAcc.accelX;
      accelerationY.number = sensorAcc.accelY;
      accelerationZ.number = sensorAcc.accelZ;
    }

  // BM1383GLV measurement was requested
  } else if((sen >> 3) & 1) {
    // Measure sensor
    state = sensorPress.measure();
    // Copy the operation result into response variable
    response |= state << 3;
    // Update error flags
    setBit(&errFlags, ERR_SENSOR_BM1383GLV, state);
    if(state == 0) {
      // Measurement was successful, update measurement variable
      pressure.number = sensorPress.pres;
    }

  // ML8511A measurement was requested
  } else if((sen >> 4) & 1) {
    // Measure sensor
    state = sensorUV.measure();
    // Copy the operation result into response variable
    response |= state << 4;
    // Update error flags
    setBit(&errFlags, ERR_SENSOR_ML8511, state);
    if(state == 0) {
      // Measurement was successful, update measurement variable
      uvRadiation.number = sensorUV.uvRad;
    }

  // BD1020HFV measurement was requested
  } else if((sen >> 5) & 1) {
    // Measure sensor
    state = sensorTmp.measure();
    // Copy the operation result into response variable
    response |= state << 5;
    // Update error flags
    setBit(&errFlags, ERR_SENSOR_BD1020HFV, state);
    if(state == 0) {
      // Measurement was successful, update measurement variable
      temperature.number = sensorTmp.temp;
    }

  // Left motor current measurement was requested
  } else if((sen >> 6) & 1) {
    // Measure current of left side motors and update the measurement variable
    currentLeft.number = analogRead(MOTOR_LEFT_CS);
    // Copy the operation result into response variable
    response |= 0x00 << 6;

  // Right motor current measurement was requested
  } else if((sen >> 7) & 1) {
    // Measure current of left side motors and update the measurement variable
    currentRight.number = analogRead(MOTOR_RIGHT_CS);
    // Copy the operation result into response variable
    response |= 0x00 << 7;
  }

  // Return operation status
  return(response);
}

