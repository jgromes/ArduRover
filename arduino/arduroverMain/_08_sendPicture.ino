// Function to send image data
byte sendPicture(byte fileNumber) {
  // Check if SD card is available
  if((errFlags & (1 << ERR_SD_NOT_FOUND))) {
    // Operation unsuccessful - SD card is not available
    return(0x03);
  }

  // Create string for the file name
  char fileName[6];

  // Get file name from specified image number
  sprintf(fileName, "%02X.JPG", fileNumber);

  // Create new file object for the image
  SdFile jpgFile;

  // Try to open the image file
  if(!jpgFile.open(fileName, O_RDWR)) {
    // Operation unsuccessful - unable to open the image file
    SD.errorHalt();
    return(0x02);
  }

  /*WIP section*/

  // Operation successful
  return(0x00);
}

