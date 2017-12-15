// Function to log measured sensor values to SD card
void logToSD(void) {
  // Create new file object for the log
  SdFile logFile;

  // Try to open the log file
  if(logFile.open(logFileName, O_CREAT | O_RDWR)) {
    // Create new String object to save the measurement values
    String line = String(magneticX.number) + "\t" + String(magneticY.number) + "\t" + String(magneticZ.number) + "\t";
    line += String(proximity.number) + "\t" + String(ambientLight.number) + "\t";
    line += String(accelerationX.number) + "\t" + String(accelerationY.number) + "\t" + String(accelerationZ.number) + "\t";
    line += String(pressure.number) + "\t";
    line += String(uvRadiation.number) + "\t";
    line += String(temperature.number) + "\t";
    // Add the log String to the log file
    logFile.println(line);
    // Close the log file
    logFile.close();
  } else {
    // Unable to open the log file
    SD.errorHalt();
  }
}

