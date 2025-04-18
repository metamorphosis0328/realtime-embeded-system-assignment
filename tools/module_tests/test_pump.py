import board
import busio
import time
from adafruit_pca9685 import PCA9685

i2c = busio.I2C(board.SCL, board.SDA)
pca = PCA9685(i2c)
pca.frequency = 50

# 通道定义
PUMP_CHANNEL = 15         # 气泵
ELECTROMAGNET_CHANNEL = 14  # 电磁铁

# 占空比设定
PWM_OFF = int(65535 * 0.025)   # 500us
PWM_ON = int(65535 * 0.125)    # 2500us

try:
    while True:
        print("气泵启动")
        pca.channels[PUMP_CHANNEL].duty_cycle = PWM_ON
        pca.channels[ELECTROMAGNET_CHANNEL].duty_cycle = PWM_OFF
        time.sleep(5)

        print("气泵关闭")
        pca.channels[PUMP_CHANNEL].duty_cycle = PWM_OFF
        time.sleep(0.5)

        print("电磁铁启动（释放棋子）")
        pca.channels[ELECTROMAGNET_CHANNEL].duty_cycle = PWM_ON
        time.sleep(1)

        print("电磁铁关闭")
        pca.channels[ELECTROMAGNET_CHANNEL].duty_cycle = PWM_OFF
        time.sleep(3)

except KeyboardInterrupt:
    print("程序中断，关闭所有通道")
    pca.channels[PUMP_CHANNEL].duty_cycle = PWM_OFF
    pca.channels[ELECTROMAGNET_CHANNEL].duty_cycle = PWM_OFF
