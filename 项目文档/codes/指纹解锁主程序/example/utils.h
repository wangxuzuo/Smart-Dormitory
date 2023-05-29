#ifndef __UTILS_H__
#define __UTILS_H__

typedef struct _Config {
  unsigned int address;
  unsigned int password;
  int has_password;
  int baudrate;
  int detect_pin;
  char serial[16];
} Config;


// 去除文件中所有空白字符(空格和制表符)
void trimSpaceInFile(const char* filename);

// 把字符串转为整型
void trim(const char* strIn, char* strOut);

// 把字符串转为整型
int toInt(const char* str);

// 把 *十六进制* 字符串转为无符号整型
unsigned int toUInt(const char* str);


#endif // __UTILS_H__
