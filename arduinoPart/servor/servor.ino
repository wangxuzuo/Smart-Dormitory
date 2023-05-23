#include <Servo.h>            //加载文件库
int pos = 0;
Servo myservo;
 
void setup()
{
  myservo.attach(9, 500, 2500);          //修正脉冲宽度
}
 
void loop()
{
  for (pos = 0; pos <= 180; pos += 1) {       //pos+=1等价于pos=pos+1
    myservo.write(pos);
             
  }
  
}
