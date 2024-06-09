#define USB_SPEED 115200

#define CAM_PCLK 33
#define CAM_XCLK 32

#define CAM_D7 4
#define CAM_D6 12
#define CAM_D5 13
#define CAM_D4 14
#define CAM_D3 15
#define CAM_D2 16
#define CAM_D1 17
#define CAM_D0 27

#define CAM_VSYNC 34
#define CAM_HREF 35

#define FRONT_ECHO 18
#define FRONT_TRIG 19

#define SIDE_ECHO 39
#define SIDE_TRIG 2

#define VNH_INA 5
#define VNH_INB 26
#define VNH_PWM 25

#define SERVO_PWM 23

const char *ssid = "...";
const char *password = "...";
const char *websocket_server = "ws://192.168.1.163:8000/ws/cleaner";
const char *endpoint = "http://192.168.1.163:8000/api/image";