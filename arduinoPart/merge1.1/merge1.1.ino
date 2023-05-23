
#include <ArduinoJson.h>



byte sensorPin = 3;
byte indicator = 13;
int red=8;
int white=7;
int fan=9;
int crushPin = 2;               // Connect sensor to input pin 2
int DoorStatus=0; //0 ->off 1->on 
int wifiState=0; //1 on $$ 0 off
int count=0;
void WIFImes(){

  
  String inString="";
  String com1="";
  while(Serial.available()>0){
    inString += char(Serial.read());
    delay(10);      // 延时函数用于等待字符完全进入缓冲区，可以尝试没有延时，输出结果会是什么
  }
  // 检查是否接收到数据，如果接收到数据，则输出该数据
  
  if(inString!=""){
    com1=inString.substring(inString.length()-5,inString.length()-2);
    Serial.print("commond:");
    Serial.println(inString);
  }
  if(com1=="tap")
  {
    count++;
    if(count==1)
     wifiState=1;
    if(count>1)
     {
       wifiState=0;
       count=0;
     }
  
  
  }

  

}
void setup()
{
  pinMode(sensorPin,INPUT);
  pinMode(indicator,OUTPUT);
  Serial.begin(115200);
  pinMode(red,OUTPUT);
  pinMode(white,OUTPUT);
  pinMode(fan,OUTPUT);
pinMode(crushPin, INPUT);   
}
void loop()
{
  byte humanState = digitalRead(sensorPin);
  byte doorState=digitalRead(crushPin);
  digitalWrite(indicator,humanState);

  WIFImes();
  //  人体检测
//   if(humanState == 1)
//   {
//     // Serial.println("Somebody is in this area!");
//     digitalWrite(red, HIGH); //将管脚设置为高电平, 则LED灯亮 
//     digitalWrite(white, HIGH); //将管脚设置为高电平, 则LED灯亮 
//     digitalWrite(fan, HIGH); //将管脚设置为高电平, 则LED灯亮 
//     Serial.print("|");// 有人
    
//   }
// if(humanState == 0&&doorState==0)
//   {
//     // Serial.println("No one!");
//    digitalWrite(red, LOW); //将管脚设置为高电平, 则LED灯亮 
//       digitalWrite(white, LOW); //将管脚设置为高电平, 则LED灯亮 
//     digitalWrite(fan, LOW); //将管脚设置为高电平, 则LED灯亮 
//       Serial.print("/");//没人
//   }

//远程控制
if(wifiState == 1)
  {
    
    digitalWrite(red, HIGH); //将管脚设置为高电平, 则LED灯亮 
    digitalWrite(white, HIGH); //将管脚设置为高电平, 则LED灯亮 
    digitalWrite(fan, HIGH); //将管脚设置为高电平, 则LED灯亮 
    Serial.print("|");// 有人
  }
  
else
{
  digitalWrite(red, LOW); 
      digitalWrite(white, LOW); 
    digitalWrite(fan, LOW); 
      Serial.print("/");//没人
}




//总和

  // if(wifiState == 1)
  // {
    
  //   digitalWrite(red, HIGH); //将管脚设置为高电平, 则LED灯亮 
  //   digitalWrite(white, HIGH); //将管脚设置为高电平, 则LED灯亮 
  //   digitalWrite(fan, HIGH); //将管脚设置为高电平, 则LED灯亮 
  //   Serial.print("|");// 有人
  // }
  // else if(humanState == 1)
  // {
  //   // Serial.println("Somebody is in this area!");
  //   digitalWrite(red, HIGH); //将管脚设置为高电平, 则LED灯亮 
  //   digitalWrite(white, HIGH); //将管脚设置为高电平, 则LED灯亮 
  //   digitalWrite(fan, HIGH); //将管脚设置为高电平, 则LED灯亮 
  //   Serial.print("|");// 有人
    
  // }

  //  else if(humanState == 0&&doorState==0)
  // {
  //   // Serial.println("No one!");
  //  digitalWrite(red, LOW); //将管脚设置为高电平, 则LED灯亮 
  //     digitalWrite(white, LOW); //将管脚设置为高电平, 则LED灯亮 
  //   digitalWrite(fan, LOW); //将管脚设置为高电平, 则LED灯亮 
  //     Serial.print("/");//没人
  // }
  // else{
  //     Serial.print("/");//没人    
  // }
  
  
  delay(5);

}