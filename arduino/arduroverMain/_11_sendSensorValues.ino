// Function to send last values of specified sensors
byte sendSensorValues(byte sen) {
  // Response variable
  byte response = errFlags & 0xFF;

  // Variable to increment position in response packet
  byte i = 1;

  // BM1422GMV values were requested
  if((sen >> 0) & 1) {
    // Copy X-axis magnetic induction value into response packet
    for(int j = 0; j < 4; j++) {
      out.data[i++] = magneticX.bytes[j];
    }
    // Copy Y-axis magnetic induction value into response packet
    for(int j = 0; j < 4; j++) {
      out.data[i++] = magneticY.bytes[j];
    }
    // Copy Z-axis magnetic induction value into response packet
    for(int j = 0; j < 4; j++) {
      out.data[i++] = magneticZ.bytes[j];
    }

  // RPR-0521RS values were requested
  } else if((sen >> 1) & 1) {
    // Copy proximity value into response packet
    for(int j = 0; j < 2; j++) {
      out.data[i++] = proximity.bytes[j];
    }
    // Copy ambient light value into response packet
    for(int j = 0; j < 4; j++) {
      out.data[i++] = ambientLight.bytes[j];
    }

  // KX022-1020 values were requested
  } else if((sen >> 2) & 1) {
    // Copy X-axis acceleration value into response packet
    for(int j = 0; j < 4; j++) {
      out.data[i++] = accelerationX.bytes[j];
    }
    // Copy Y-axis acceleration value into response packet
    for(int j = 0; j < 4; j++) {
      out.data[i++] = accelerationY.bytes[j];
    }
    // Copy Z-axis acceleration value into response packet
    for(int j = 0; j < 4; j++) {
      out.data[i++] = accelerationZ.bytes[j];
    }

  // BM1383GLV values were requested
  } else if((sen >> 3) & 1) {
    // Copy pressure value into response packet
    for(int j = 0; j < 4; j++) {
      out.data[i++] = pressure.bytes[j];
    }

  // ML8511A values were requested
  } else if((sen >> 4) & 1) {
    // Copy UV radiation intensity value into response packet
    for(int j = 0; j < 4; j++) {
      out.data[i++] = uvRadiation.bytes[j];
    }

  // BD1020HFV values were requested
  } else if((sen >> 5) & 1) {
    // Copy temperature value into response packet
    for(int j = 0; j < 4; j++) {
      out.data[i++] = temperature.bytes[j];
    }
  }

  // Return operation status
  return(response);
}

