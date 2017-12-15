// RohmMultiSensor sensors
#define INCLUDE_BM1422GMV
#define INCLUDE_RPR_0521RS
#define INCLUDE_KX022_1020
#define INCLUDE_BM1383GLV
#define INCLUDE_ML8511A
#define INCLUDE_BD1020HFV

// Libraries
#include <RohmMultiSensor.h>
#include <LoRaLib.h>
#include <JPEGDecoder.h>
#include <Adafruit_VC0706.h>
#include <Servo.h>

// Remote control commands
#define CMD_MOTOR_LR_STOP           0x00
#define CMD_MOTOR_LR_SPEED          0x10
#define CMD_MOTOR_L_SPEED           0x20
#define CMD_MOTOR_L_CURRENT         0x30
#define CMD_MOTOR_R_SPEED           0x40
#define CMD_MOTOR_R_CURRENT         0x50
#define CMD_CAMERA_TILT             0x60
#define CMD_CAMERA_PAN              0x70
#define CMD_CAMERA_SNAP             0x80
#define CMD_CAMERA_SEND             0x90
#define CMD_SENSOR_MEASURE          0xA0
#define CMD_SENSOR_SEND             0xB0
#define CMD_LORA_RESEND_LAST        0xC0
#define CMD_LORA_CONFIG             0xD0
#define CMD_LORA_REQUEST_ERRORS     0xE0

// Error codes
#define ERR_SD_NOT_FOUND            0x00
#define ERR_CAMERA_NOT_FOUND        0x01
#define ERR_SENSOR_BM1422GMV        0x02
#define ERR_SENSOR_RPR_0521RS       0x03
#define ERR_SENSOR_KX022_1020       0x04
#define ERR_SENSOR_BM1383GLV        0x05
#define ERR_SENSOR_ML8511           0x06
#define ERR_SENSOR_BD1020HFV        0x07

// Pin connections
#define MOTOR_LEFT_INA              41
#define MOTOR_LEFT_INB              43
#define MOTOR_LEFT_PWM              45
#define MOTOR_LEFT_CS               A15
#define MOTOR_RIGHT_INA             40
#define MOTOR_RIGHT_INB             42
#define MOTOR_RIGHT_PWM             44
#define MOTOR_RIGHT_CS              A14
#define SD_CS                       10
#define SD_SCK                      11
#define SD_MOSI                     12
#define SD_MISO                     13
#define CAMERA_PAN                  9
#define CAMERA_TILT                 8
#define RSS_INT_1                   18
#define RSS_INT_2                   19

// Motors
#define MOTOR_LEFT                  0
#define MOTOR_RIGHT                 1
#define MOTOR_BOTH                  2

// Servo positions
#define PAN_MIN                     10
#define PAN_MID                     85
#define PAN_MAX                     170
#define TILT_MIN                    130
#define TILT_MID                    130
#define TILT_MAX                    10

