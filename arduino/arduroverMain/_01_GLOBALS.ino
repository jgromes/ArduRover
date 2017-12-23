// RohmMultiSensor sensor class instances
BM1422GMV sensorMag(digitalPinToInterrupt(RSS_INT_1));
RPR_0521RS sensorAls;
KX022_1020 sensorAcc(digitalPinToInterrupt(RSS_INT_2));
BM1383GLV sensorPress;
ML8511A sensorUV(ANALOG_2);
BD1020HFV sensorTmp(ANALOG_1);

// Interrupt service routine for accelerometer
void acc_isr(void) {
  sensorAcc.setFlagDrdy();
}

// Interrupt service routine for magnetometer
void mag_isr(void) {
  sensorMag.setFlagDrdy();
}

// Union type to easily access bytes in float
typedef union {
  float number;
  byte bytes[4];
} uni_float_byte_t;

// Union type to easily access bytes in int
typedef union {
  int number;
  byte bytes[2];
} uni_int_byte_t;

// Sensor measurement variables
uni_float_byte_t magneticX;
uni_float_byte_t magneticY;
uni_float_byte_t magneticZ;
uni_int_byte_t proximity;
uni_float_byte_t ambientLight;
uni_float_byte_t accelerationX;
uni_float_byte_t accelerationY;
uni_float_byte_t accelerationZ;
uni_float_byte_t pressure;
uni_float_byte_t uvRadiation;
uni_float_byte_t temperature;
uni_int_byte_t currentLeft;
uni_int_byte_t currentRight;

// LoRa shield instance
LoRa lora;

// Packet instances
uint8_t cmd[4] = {0, 0, 0, 0};
Packet in("01:23:45:67:89:AB:CD:EF", cmd, 4);
Packet out("01:23:45:67:89:AB:CD:EF", cmd, 4);
Packet last("01:23:45:67:89:AB:CD:EF", cmd, 4);

// Servo instance
Servo servo;

// VC0706 camera class instance
Adafruit_VC0706 cam = Adafruit_VC0706(&Serial2);

// Error flags
unsigned int errFlags = 0;

// SD card
SdFatSoftSpi<SD_MISO, SD_MOSI, SD_SCK> SD;

// Name of the log file
const char logFileName[] = "datalog.txt";

// Function prototypes
byte stopMotors(void);
byte setMotorSpeed(byte motor, byte pwm, byte dir);
byte takePicture(void);
byte sendPicture(byte fileNumber);
byte measureSensorValues(byte sen);
void logToSD(void);
byte sendSensorValues(byte sen);
void setServoPosition(int pin, int angle);

