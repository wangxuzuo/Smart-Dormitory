#include "../as608.h"
#include "./utils.h"

#include <wiringPi.h>
#include <wiringSerial.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

extern AS608 g_as608;
extern int g_fd;
extern int g_verbose;
extern char  g_error_desc[128];
extern uchar g_error_code;

int  g_argc = 0;   // 参数个数，g_argc = argc - g_option_count
int  g_option_count = 0; // 选项个数-v、-h等
char g_command[16] = { 0 };     // 即argv[1]
Config g_config;   // 配置文件 结构体，定义在"./utils.h"头文件中

void printConfig();
void printUsage();
bool readConfig();  // 读取文件到 g_config
bool writeConfig(); // 将 g_config 写入文件
void asyncConfig();
void priorAnalyseArgv(int argc, char* argv[]);
void analyseArgv(int argc, char* argv[]);

bool waitUntilDetectFinger(int wait_time);   // 阻塞至检测到手指，最长阻塞wait_time毫秒
bool waitUntilNotDetectFinger(int wait_time);
void checkFinger();

// 因为as608.h内的函数执行失败而退出程序
bool PS_Exit() {
  printf("ERROR! code=%02X, desc=%s\n", g_error_code, PS_GetErrorDesc());
  exit(2);
  return true;
}

// 程序退出时执行的工作，关闭串口等
void atExitFunc() {
  if (g_verbose == 1)
    printf("Exit\n");
  if (g_fd > 0)
    serialClose(g_fd); 
}


/*******************************************************************
 *
 * main()
 *
*******************************************************************/

int main(int argc, char *argv[]) //int serialOpen (const char *device, const int baud)
{
  // 1.读取配置文件，获得芯片地址和通信密码
  if (!readConfig())
    exit(1);

  // 2.优先解析的内容，如参数选项、配置本地文件等
  priorAnalyseArgv(argc, argv);
  
  if (g_verbose == 1)
    printConfig();
  
  // 3.初始化wiringPi库
  if (-1 == wiringPiSetup()) {
    printf("wiringPi setup failed!\n");
    return 1;
  }

  // 4.检测是否有手指放上的GPIO端口，设为输入模式
  pinMode(g_config.detect_pin, INPUT);

  // 5.打开串口
	if((g_fd = serialOpen(g_config.serial, g_config.baudrate)) < 0)	{
		fprintf(stderr,"Unable to open serial device: %s\n", strerror(errno));
		return 1;
	}

  // 6.注册退出函数(打印一些信息、关闭串口等)
  atexit(atExitFunc);

  // 7.初始化 AS608 模块
  // 地址 密码
  PS_Setup(g_config.address, g_config.password) ||  PS_Exit();

  // 8.主处理函数，解析普通命令(argv[1])，
  analyseArgv(argc, argv);
  //checkFinger();
	return 0;
}


/***************************************************************
 *
 * 函数定义
 *
***************************************************************/
/*void checkFinger(){
    while(1){
    //checkArgc(2);

    printf("Please put your finger on the module.\n");
    delay(2000);
    PS_GetImage(); 
    PS_GenChar(1);

    int pageID = 0, score = 0;
    if (!PS_Search(1, 0, 300, &pageID, &score))
      continue;
    else{
      int PWM_pin = 1;  
      int intensity ;            

      if (wiringPiSetup () == -1)
        exit (1) ;

    pinMode (PWM_pin, PWM_OUTPUT) ;
    int time = 1;
    while (time--)
    {

      for (intensity = 0 ; intensity < 1024 ; ++intensity)
      {
        pwmWrite (PWM_pin, intensity) ; 
        delay (1) ;
      }
      delay(1);

      for (intensity = 1023 ; intensity >= 0 ; --intensity)
      {
        pwmWrite (PWM_pin, intensity) ;
        delay (1) ;
      }
      delay(1);
      printf("Matched! pageID=%d score=%d\n", pageID, score);
      
    }
    break;
      }
  }
}*/

// 检查参数数量是否正确是否
bool checkArgc(int argcNum) {
  if (argcNum == g_argc)
    return true;
  else if (argcNum == 2)
    printf("ERROR! \"%s\" accept no parameter\n", g_command);
  else if (argcNum == 3)
    printf("ERROR! \"%s\" accept 1 parameter\n", g_command);
  else if (argcNum > 3)
    printf("ERROR! \"%s\" accept %d parameters\n", g_command, argcNum);

  exit(1);
}

// 匹配argv[1], 即g_command
bool match(const char* str) {
  return strcmp(str, g_command) == 0;
}

// 需要优先解析的命令，如配置文件的修改、选项解析等
// 不需要与模块通信
void priorAnalyseArgv(int argc, char* argv[]) {
  if (argc < 2) {
    printUsage();
    exit(1);
  }

  // 检查选项  -v -h
  for (int i = 0; i < argc; ++i) {
    if (strcmp(argv[i], "-h") == 0) {
      printUsage();
      g_option_count++;
      exit(0);
    }
    else if (strcmp(argv[i], "-v") == 0) {
      g_verbose = 1;
      g_option_count++;
    }
  }
  
  g_argc = argc - g_option_count;
  strcpy(g_command, argv[1]);

  if (match("cfg")) {
    printConfig();
    exit(0);
  }

  // 配置通信地址
  else if (match("cfgaddr")) {
    checkArgc(3);
    g_config.address = toUInt(argv[2]);
    writeConfig();
    exit(0);
  }

  // 配置通信密码
  else if (match("cfgpwd")) {
    checkArgc(3);
    g_config.password = toUInt(argv[2]);
    g_config.has_password = 1;
    writeConfig();
    exit(0);
  }

  // 配置串口号
  else if (match("cfgserial")) {
    checkArgc(3);
    strcpy(g_config.serial, argv[2]);
    writeConfig();
    exit(0);
  }

  else if (match("cfgbaud")) {
    checkArgc(3);
    g_config.baudrate = toInt(argv[2]);
    writeConfig();
    exit(0);
  }

  else if (match("cfgpin")) {
    checkArgc(3);
    g_config.detect_pin = toInt(argv[2]);
    writeConfig();
    exit(0);
  }
}

// 阻塞至检测到手指，最长阻塞wait_time毫秒
bool waitUntilDetectFinger(int wait_time) {
  while (true) {
    if (PS_DetectFinger()) {
      return true;
    }
    else {
      delay(100);
      wait_time -= 100;
      if (wait_time < 0) {
        return false;
      }
    }
  }
}

bool waitUntilNotDetectFinger(int wait_time) {
  while (true) {
    if (!PS_DetectFinger()) {
      return true;
    }
    else {
      delay(100);
      wait_time -= 100;
      if (wait_time < 0) {
        return false;
      }
    }
  }
}


// 主处理函数，解析命令
void analyseArgv(int argc, char* argv[]) {

  if (match("add")) {
    checkArgc(3);
    printf("Please put your finger on the module.\n");
    if (waitUntilDetectFinger(5000)) {
      delay(500);
      PS_GetImage() || PS_Exit();
      PS_GenChar(1) || PS_Exit();
    }
    else {
      printf("Error: Didn't detect finger!\n");
      exit(1);
    }

    // 判断用户是否抬起了手指，
    printf("Ok.\nPlease raise your finger!\n");
    if (waitUntilNotDetectFinger(5000)) {
      delay(100);
      printf("Ok.\nPlease put your finger again!\n");
      // 第二次录入指纹
      if (waitUntilDetectFinger(5000)) {
        delay(500);
        PS_GetImage() || PS_Exit();
        PS_GenChar(2) || PS_Exit();
      }
      else {
        printf("Error: Didn't detect finger!\n");
        exit(1);
      }
    }
    else {
      printf("Error! Didn't raise your finger\n");
      exit(1);
    }

    int score = 0;
    if (PS_Match(&score)) {
      printf("Matched! score=%d\n", score);
    }
    else {
      printf("Not matched, raise your finger and put it on again.\n");
      exit(1);
    }
    
    if (g_error_code != 0x00)
      PS_Exit();

    // 合并特征文件
    PS_RegModel() || PS_Exit();
    PS_StoreChar(2, toInt(argv[2])) || PS_Exit();

    printf("OK! New fingerprint saved to pageID=%d\n", toInt(argv[2]));
  }





  else if (match("delete")) {
    int startPageID = 0;
    int count = 0;

    // 判断参数个数
    if (g_argc == 3) {
      startPageID = toInt(argv[2]);
      count = 1;
      printf("Confirm to delete fingerprint %d: (Y/n)? ", startPageID);
    }
    else if (argc == 4) {
      startPageID = toInt(argv[2]);
      count = toInt(argv[3]);
      printf("Confirm to delete fingerprint %d-%d: (Y/n)? ", startPageID, startPageID+count-1);
    }
    else {
      printf("Command \"delete\" accept 1 or 2 parameter\n");
      printf("  Usage: fp delete startPageID [count]\n");
      exit(1);
    }

    // 询问是否继续
    fflush(stdout);
    char cmd = getchar();
    if (cmd == 'n' || cmd ==  'N') {
      printf("Canceled!\n");
      exit(0);
    }

    PS_DeleteChar(startPageID, count) || PS_Exit();
    printf("OK!\n");
  }

  else if (match("search")) {
    //checkArgc(2);
     pinMode(27,OUTPUT);
     pinMode(26,OUTPUT);
    while(1){
    printf("Please put your finger on the module.\n");
    delay(2000);
    PS_GetImage(); 
    PS_GenChar(1);

    int pageID = 0, score = 0;
    if (!PS_Search(1, 0, 300, &pageID, &score)){
      //PS_Exit();
      digitalWrite(27,1);
      
      continue;
      }
    else{
      int PWM_PIN = 1;
      wiringPiSetup();
      digitalWrite(27,0);
      digitalWrite(26,1);
      sleep(1);
      digitalWrite(26,0);
      pinMode(PWM_PIN, PWM_OUTPUT); // 设置PWM输出
      pwmSetMode(PWM_MODE_MS); // 设置传统模式
      pwmSetClock(192); // 设置分频
      pwmSetRange(2000); // 设置周期分为2000步
      pwmWrite(PWM_PIN, 200);
      delay(900);
      pwmWrite(PWM_PIN, 150);
      delay(3000);
      pwmWrite(PWM_PIN, 100);
      delay(900);
      pwmWrite(PWM_PIN, 150);
      printf("Matched! pageID=%d score=%d\n", pageID, score);
    }
      
    }
    

   
      
  }


  // 列出指纹列表
  else if (match("list")) {
    checkArgc(2);
    int indexList[512] = { 0 };
    PS_ReadIndexTable(indexList, 512) ||  PS_Exit();

    int i = 0;
    for (i = 0; i < 300; ++i) {
      if (indexList[i] == -1)
        break;
      printf("%d\n", indexList[i]);
    }
    if (i == 0) {
      printf("The database is empty!\n");
    }
  }





  else {
    printf("Unknown parameter \"%s\"\n", argv[1]);
    exit(1);
  }
} // end analyseArgv


// 打印配置文件内容到屏幕上
void printConfig() {
  printf("address=%08x\n", g_config.address);
  if (g_config.has_password)
    printf("password=%08x\n", g_config.password);
  else
    printf("password=none(no password)\n");
  printf("serial_file=%s\n",   g_config.serial);
  printf("baudrate=%d\n",   g_config.baudrate);
  printf("detect_pin=%d\n",   g_config.detect_pin);
}

// 同步g_config变量内容和其他变量内容
void asyncConfig() {
  g_as608.detect_pin   = g_config.detect_pin;
  g_as608.has_password = g_config.has_password;
  g_as608.password     = g_config.password;
  g_as608.chip_addr    = g_config.address;
  g_as608.baud_rate    = g_config.baudrate;
}

// 读取配置文件
bool readConfig() {
  FILE* fp;

  // 获取用户主目录
  char filename[256] = { 0 };
  sprintf(filename, "%s/.fpconfig", getenv("HOME"));
  
  // 主目录下的配置文件
  if (access(filename, F_OK) == 0) { 
    trimSpaceInFile(filename);
    fp = fopen(filename, "r");
  }
  else {
    // 如果配置文件不存在，就在主目录下创建配置文件，并写入默认配置
    // 设置默认值
    g_config.address = 0xffffffff;
    g_config.password= 0x00000000;
    g_config.has_password = 0;
    g_config.baudrate = 9600;
    g_config.detect_pin = 1; 
    strcpy(g_config.serial, "/dev/ttyAMA0");

    writeConfig();

    printf("Please config the address and password in \"~/.fpconfig\"\n");
    printf("  fp cfgaddr   0x[address]\n");
    printf("  fp cfgpwd    0x[password]\n");
    printf("  fp cfgserial [serialFile]\n");
    printf("  fp cfgbaud   [rate]\n");
    printf("  fp cfgpin    [GPIO_pin]\n");
    return false;
  }

  char key[16] = { 0 };
  char value[16] = { 0 };
  char line[32] = { 0 };

  char *tmp;
  while (!feof(fp)) {
    fgets(line, 32, fp);
    
    // 分割字符串，得到key和value
    if (tmp = strtok(line, "="))
      trim(tmp, key);
    else
      continue;
    if (tmp = strtok(NULL, "="))
      trim(tmp, value);
    else
      continue;
    while (!tmp)
      tmp = strtok(NULL, "=");

    // 如果数值以 0x 开头
    int offset = 0;
    if (value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
      offset = 2;

    if (strcmp(key, "address") == 0) {
      g_config.address = toUInt(value+offset);
    }
    else if (strcmp(key, "password") == 0) {
      if (strcmp(value, "none") == 0 || strcmp(value, "false") == 0) {
        g_config.has_password = 0; // 无密码
      }
      else {
        g_config.has_password = 1; // 有密码
        g_config.password = toUInt(value+offset);
      }
    }
    else if (strcmp(key, "serial") == 0) {
      int len = strlen(value);
      if (value[len-1] == '\n')
        value[len-1] = 0;
      strcpy(g_config.serial, value);
    }
    else if (strcmp(key, "baudrate") == 0) {
      g_config.baudrate = toInt(value);
    }
    else if (strcmp(key, "detect_pin") == 0) {
      g_config.detect_pin = toInt(value);
    }
    else {
      printf("Unknown key:%s\n", key);
      fclose(fp);
      return false;
    }

  } // end while(!feof(fp))

  asyncConfig();

  fclose(fp);
  return true;
}

/*
 * 写配置文件
*/
bool writeConfig() {
  // 获取用户主目录
  char filename[256] = { 0 };
  sprintf(filename, "%s/.fpconfig", getenv("HOME"));
  
  FILE* fp = fp = fopen(filename, "w+");
  if (!fp) {
    printf("Write config file error!\n");
    exit(0);
  }

  fprintf(fp, "address=0x%08x\n", g_config.address);
  if (g_config.has_password)
    fprintf(fp, "password=0x%08x\n", g_config.password);
  else
    fprintf(fp, "password=none\n");
  fprintf(fp, "baudrate=%d\n", g_config.baudrate);
  fprintf(fp, "detect_pin=%d\n", g_config.detect_pin);
  fprintf(fp, "serial=%s\n", g_config.serial);

  fclose(fp);
}


// 打印程序使用说明
void printUsage() {
  printf("A command line program to interact with AS608 module.\n\n");
  printf("Usage:\n  ./fp [command] [param] [option]\n");
  printf("\nAvailable Commands:\n");

  printf("-------------------------------------------------------------------------\n");
  printf("  command  | param     | description\n");
  printf("-------------------------------------------------------------------------\n");
  printf("  cfgaddr   [addr]     Config address in local config file\n");
  printf("  cfgpwd    [pwd]      Config password in local config file\n");
  printf("  cfgserial [serialFile] Config serial port in local config file. Default:/dev/ttyAMA0\n");
  printf("  cfgbaud   [rate]     Config baud rate in local config file\n");
  printf("  cfgpin    [GPIO_pin] Config GPIO pin to detect finger in local confilg file\n\n");
  printf("  add       [pID]      Add a new fingerprint to database. (Read twice) \n");
  printf("  enroll    []         Add a new fingerprint to database. (Read only once)\n");
  printf("  search    []         Collect fingerprint and search in database.\n");
  printf("  list      []         Show the registered fingerprints list.\n");

  
  printf("\nAvaiable options:\n");
  printf("  -h    Show help\n");
  printf("  -v    Shwo details while excute the order\n");

  printf("\nUsage:\n  ./fp [command] [param] [option]\n\n");
}

