void setup() {
  // Start I2C communication for sensors
  Wire.begin();

  // Set all pin modes
  pinMode(MOTOR_LEFT_INA, OUTPUT);
  pinMode(MOTOR_LEFT_INB, OUTPUT);
  pinMode(MOTOR_LEFT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_CS, INPUT);
  pinMode(MOTOR_RIGHT_INA, OUTPUT);
  pinMode(MOTOR_RIGHT_INB, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_RIGHT_CS, INPUT);
  pinMode(SD_CS, OUTPUT);
  pinMode(CAMERA_TILT, INPUT);
  pinMode(CAMERA_PAN, INPUT);

  // Make sure that motors are stopped
  stopMotors();

  // Disable both SPI devices (SD card and LoRenz shield)
  //digitalWrite(SD_CS, HIGH);
  //digitalWrite(7, HIGH);

  // Initialize sensors and set the initialization result to error flags
  setBit(&errFlags, ERR_SENSOR_BM1422GMV, sensorMag.init(mag_isr));
  setBit(&errFlags, ERR_SENSOR_RPR_0521RS, sensorAls.init());
  setBit(&errFlags, ERR_SENSOR_KX022_1020, sensorAcc.init(acc_isr));
  setBit(&errFlags, ERR_SENSOR_BM1383GLV, sensorPress.init());
  setBit(&errFlags, ERR_SENSOR_ML8511, sensorUV.init());
  setBit(&errFlags, ERR_SENSOR_BD1020HFV, sensorTmp.init());

  // Initialize the union-type sensor measurement variables
  magneticX.number = 0;
  magneticY.number = 0;
  magneticZ.number = 0;
  proximity.number = 0;
  ambientLight.number = 0;
  accelerationX.number = 0;
  accelerationY.number = 0;
  accelerationZ.number = 0;
  pressure.number = 0;
  uvRadiation.number = 0;
  temperature.number = 0;

  // Set out packet destination address
  out.setDestinationStr("01:23:45:67:89:AB:CD:EF");

  Serial.begin(9600);

  // Start LoRenz shield
  if(lora.begin() != ERR_NONE) {
    // LoRenz shield is a critical system, if it was not found, program will halt
    while(true);
  }

  // Start SD card
  if(!SD.begin(SD_CS)) {
    // If the SD card was not found, set error bit
    errFlags |= 1 << ERR_SD_NOT_FOUND;
    SD.initErrorHalt();
    // SD module is a non-critical system, we can continue without it, but values won't be logged and camera won't work
  } else {
    // Start VC0706 camera if the SD was found
    if(!cam.begin()) {
      // If the camera was not found, set error bit
      errFlags |= 1 << ERR_CAMERA_NOT_FOUND;
      // Camera is a non-critical system, we can continue without it
    } else {
      // Set picture resolution
      cam.setImageSize(VC0706_640x480);
    }
  }
  
  // Set default servo positions
  setServoPosition(CAMERA_PAN, PAN_MID);
  setServoPosition(CAMERA_TILT, TILT_MID);
}

