#ifndef PORT_DEF_H
#define PORT_DEF_H

constexpr static int PORT_LED1 = 12;
constexpr static int PORT_LED2 = 14;
constexpr static int PORT_SW1 = 26;
constexpr static int PORT_SW2 = 27;
constexpr static int PORT_BUZZER = 25;
constexpr static int PORT_DIPSW1 = 32;
constexpr static int PORT_DIPSW2 = 33;
constexpr static int PORT_DOOR_SW = 15;
constexpr static int PORT_LOCK_DET = 13;
constexpr static int PORT_SERVO_POW = 17;
constexpr static int PORT_SERVO_SIG = 16;
constexpr static int PORT_PN532_POW = 4;
constexpr static int PORT_PN532_MOSI = 23;
constexpr static int PORT_PN532_MISO = 19;
constexpr static int PORT_PN532_SCK = 18;
constexpr static int PORT_PN532_CS = 5;
constexpr static int PORT_I2C_SCL = 22;
constexpr static int PORT_I2C_SDA = 21;

constexpr static int SERVO_LEDC_CHANNEL = 0;
constexpr static int BUZZER_LEDC_CHANNEL = 1;

#endif