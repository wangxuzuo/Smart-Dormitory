#!/usr/bin/env python
# -*- coding: utf-8 -*-

__author__ = "stao"

from blinker import Device, ButtonWidget, NumberWidget
import wiringpi
device = Device("3f07e35c3649")

INPUT = 0
OUTPUT = 1
PWM_OUTPUT = 2
GPIO_CLOCK = 3
SOFT_PWM_OUTPUT = 4
SOFT_TONE_OUTPUT= 5
PWM_TONE_OUTPUT = 6
LOW = 0
HIGH = 1
PWM_MODE_MS = 0
PWM_MODE_BAL = 1


button1 = device.addWidget(ButtonWidget('btn-123'))
button2 = device.addWidget(ButtonWidget('btn-abc'))
number1 = device.addWidget(NumberWidget('num-abc'))

num = 0


async def button1_callback(msg):
    global num

    num += 1

    await number1.text("num").value(num).update()


async def button2_callback(msg):
    print("Button2: {0}".format(msg))
    PWM_PIN = 1
    wiringpi.wiringPiSetup() # WiringPi初始化
    wiringpi.pinMode(PWM_PIN, PWM_OUTPUT) # 设置引脚为PWM模式
    wiringpi.pwmSetMode(PWM_MODE_MS) # 修改PWM为传统模式
    wiringpi.pwmSetClock(192) # 设置分频系数
    wiringpi.pwmSetRange(2000) # 设置周期步长(占空比范围)
    wiringpi.pwmWrite(PWM_PIN, 200)
    wiringpi.delay(800)
    wiringpi.pwmWrite(PWM_PIN, 150)
    wiringpi.delay(3000)
    wiringpi.pwmWrite(PWM_PIN, 100)
    wiringpi.delay(730)
    wiringpi.pwmWrite(PWM_PIN, 150)
    
    

async def heartbeat_func(msg):
    print("Heartbeat func received: {0}".format(msg))
    # 文本组件


async def ready_func():
    # 获取设备配置信息
    print(vars(device.config))


button1.func = button1_callback
button2.func = button2_callback

device.heartbeat_callable = heartbeat_func
device.ready_callable = ready_func

if __name__ == '__main__':
    device.run()
