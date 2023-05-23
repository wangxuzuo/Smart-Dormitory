#define BLINKER_PRINT Serial
#define BLINKER_WIFI

#include <Blinker.h>

char auth[] = "86f3b7d0d871";
char ssid[] = "9721_2.4G";
char pswd[] = "97219721";
// char ssid[] = "wangxuzuo";
// char pswd[] = "20011002";

// 新建组件对象
BlinkerButton Button1("btn-abc");
BlinkerNumber Number1("num-abc");
BlinkerText Text1("tex-dui");
int counter = 0;

// 按下按键即会执行该函数
void button1_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}



// 如果未绑定的组件被触发，则会执行其中内容
void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);
    counter++;
    Number1.print(counter);

    Blinker.vibrate();
    
    uint32_t BlinkerTime = millis();
    
    Blinker.print("millis", BlinkerTime);
  
   if(char(Serial.read())=='|')
    Text1.print("厕所状态", "有人");
    else if(char(Serial.read())=='/')
    Text1.print("厕所状态","没人");
    
    
}

void setup() {
    // 初始化串口
    Serial.begin(115200);

    #if defined(BLINKER_PRINT)
        BLINKER_DEBUG.stream(BLINKER_PRINT);
    #endif
    
    // 初始化有LED的IO
    pinMode(LED_BUILTIN, OUTPUT);
    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);
    Blinker.attachData(dataRead);
    Button1.attach(button1_callback);
    
}

void loop() {
    Blinker.run();
    // Text1.print("text1", "text2");
}

