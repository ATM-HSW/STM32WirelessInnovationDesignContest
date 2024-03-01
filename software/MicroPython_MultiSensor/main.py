from machine import I2C
from mpy_htu21 import mpy_htu21
from mpy_bme68x import mpy_bme68x
from mpy_sgp40 import mpy_sgp40


i2c=I2C(scl='B8', sda='B9')
i2c.scan()

htu21=mpy_htu21(i2c)
print("htu21")
print(htu21.temperature)
print(htu21.humidity)

bme=mpy_bme68x(i2c)
print("bme68x")
print(bme.temperature)
print(bme.humidity)
print(bme.pressure)
print(bme.gas)

sgp=mpy_sgp40(i2c)
print("sgp40")
print(sgp.measure_raw(htu21.humidity, htu21.temperature))