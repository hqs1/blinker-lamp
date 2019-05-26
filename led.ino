#define BLINKER_PRINT Serial //串口调试组件
#define BLINKER_MQTT         //通讯方式
#define BLINKER_ALIGENIE_LIGHT

//#define BLINKER_ESP_SMARTCONFIG
#define BLINKER_DEBUG_ALL
#include <Blinker.h>

#define LED D1
#define Button_io D2
#define LED_light_MAX 100
#define LED_light_MIX 10

bool LED_power = false; //灯状态
int  set_light = 60;    //设置态
int  Now_light = 0;     //现在态

char auth[] = "";     //APP key
char ssid[] = "";        //WiFi 名字
char pswd[] = "";       //密码
//创建对象
BlinkerButton power("btn_pwr");
BlinkerSlider Slider1("Slider");

/*
 * APP组件
 */
void slider1_callback(int32_t value) //滑动条处理
{
  set_light = value;
  LED_light_set(set_light , LED_power);
  Slider1.print(value);
}

void power_callback(const String &state) { //台灯开关处理
  if (state == BLINKER_CMD_ON) {
    LED_power = true ;
    power.icon("fal fa-power-off");
    power.color("#FF0000");
    power.text("", "开");
    power.print("on");
  }
  if (state == BLINKER_CMD_OFF) {
    LED_power = false ;
    power.icon("fal fa-power-off");
    power.color("#FFFFFF");
    power.text("", "关");
    power.print("off");
  }
  LED_light_set(set_light , LED_power);
}

/*
 * 语音音箱处理
 */
void aligeniePowerSate(const String & state) //台灯开关处理
{
  if (state == BLINKER_CMD_ON) {
    LED_power = true ;
    BlinkerAliGenie.powerState("on");
    BlinkerAliGenie.print();
  }
  else if (state == BLINKER_CMD_OFF) {
    LED_power = false ;
    BlinkerAliGenie.powerState("off");
    BlinkerAliGenie.print();
  }
  LED_light_set(set_light , LED_power);
}


void aligenieBright(const String & bright) //灯亮度处理
{
  if (bright == BLINKER_CMD_MAX) {
    set_light = LED_light_MAX;
  }
  else if (bright == BLINKER_CMD_MIN) {
    set_light = LED_light_MIX;
  }
  else {

    set_light = bright.toInt();
  }
  BlinkerAliGenie.brightness(set_light);
  BlinkerAliGenie.print();
  LED_light_set(set_light , LED_power);

}

void aligenieRelativeBright(int32_t bright) 
{
  set_light += bright;
  BlinkerAliGenie.brightness(set_light);
  BlinkerAliGenie.print();
  LED_light_set(set_light , LED_power);

}



void LED_light_set(int light, bool power) {
  light = light * 10.24;
  if (power) {
    analogWrite(LED ,light);
  }
  else {
    analogWrite(LED , LOW);
  }
}





void setup()
{
  Serial.begin(9600);
#if defined(BLINKER_PRINT)
  BLINKER_DEBUG.stream(BLINKER_PRINT);
  BLINKER_DEBUG.debugAll();
#endif
  power.attach(power_callback);
  Slider1.attach(slider1_callback);
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachHeartbeat(heartbeat);
  BlinkerAliGenie.attachPowerState(aligeniePowerSate);
  BlinkerAliGenie.attachBrightness(aligenieBright);
  BlinkerAliGenie.attachRelativeBrightness(aligenieRelativeBright);

  pinMode(LED , OUTPUT);
  pinMode(Button_io , INPUT_PULLUP);
  digitalWrite(LED, LOW);
  attachInterrupt(Button_io,button,FALLING);
}

void loop() {
  Blinker.run();
}



void heartbeat() //心跳包
{
  if (LED_power) {
    power.icon("fal fa-power-off");
    power.color("#FF0000");
    power.text("", "开");
    power.print("on");
  }
  else {
    power.icon("fal fa-power-off");
    power.color("#FFFFFF");
    power.text("", "关");
    power.print("off");
  }
  Slider1.print(set_light);
}



void button() {
    if (digitalRead(Button_io) == LOW){ 
      detachInterrupt(Button_io);     
      LED_power = !LED_power;
      LED_light_set(set_light , LED_power);
    }
    attachInterrupt(Button_io,button,FALLING);
}
