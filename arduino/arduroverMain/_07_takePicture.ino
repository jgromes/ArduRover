// Function to take picture with camera
byte takePicture(void) {
  // Check if camera is available
  if((errFlags & (1 << ERR_CAMERA_NOT_FOUND))) {
    // Operation unsuccessful - camera is not available
    return(0x03);
  }

  // Try to take a picture
  if(!cam.takePicture()) {
    // Operation unsuccessful - unable to take a picture
    return(0x01);
  }

  // Create a string to save the file name
  char fileName[13];
  strcpy(fileName, "xx.JPG");

  // Create unique file name for the picture
  for (int i = 0; i < 100; i++) {
    fileName[0] = '0' + i/10;
    fileName[1] = '0' + i%10;
    if (!SD.exists(fileName)) {
      break;
    }
  }

  // Create new file object for the image
  SdFile imgFile;
  
  // Try to open the image file
  if(!imgFile.open(fileName,  O_CREAT | O_RDWR)) {
    // Operation unsuccessful - unable to save the picture
    SD.errorHalt();
    return(0x02);
  }

  // Get number of pixels in the image
  uint16_t jpglen = cam.frameLength();

  // Read all the image data
  while (jpglen > 0) {
    // Load the JPEG-encoded image data from the camera into a buffer
    uint8_t *buff;
    uint8_t bytesToRead = min(32, jpglen);
    buff = cam.readPicture(bytesToRead);

    // Write the image data to the file
    imgFile.write(buff, bytesToRead);
    jpglen -= bytesToRead;
  }

  // Close the image file
  imgFile.close();

  // Operation successful
  return(0x00);
}

