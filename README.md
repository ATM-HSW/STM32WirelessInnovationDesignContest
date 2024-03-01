# Universal room sensor for various MCUs with communication interfaces

## Teaser:

As part of this project, a multi-sensor module is being developed with which various room parameters can be measured. In addition to the usual measured values of temperature, humidity and air pressure, the indoor air quality is also to be determined using VOC and CO2 sensors. A multi-channel light measurement is also to be carried out.

The sensor module should be as compact as possible and be able to be used universally on various MCU platforms via the I2C bus.

Programming should be as simple as possible using existing libraries and software environments, e.g. Arduino environment, Micro- or CircuitPython.

The choice in this competition fell on the relatively new STM32WB52xx platform. Experience with other STM Nucleo boards forms the basis for this project.

## Project Details

The universal room sensor is being developed as the most universal platform possible to measure the parameters of the environment, i.e. rooms such as a lecture theatres, seminar rooms or offices. It should also be possible to test different MCU variants with different communication interfaces and different software environments.

The aim is to equip the rooms of a university with the room sensor. Staff and students should be able to access the sensor values. In future, however, it should also be possible to use the values for needs-based and energy-efficient control of the rooms and integration into the Building Information Management (BIM) system.

![[images/pic1.jpg]]


## System Overview

As the chosen platform Nucleo-WBA52 is still relatively new, the software support is not yet as extensive as with hardware platforms that have been available for longer. The aim of this project is not the direct use of STM's Cube Packages but of development environments such as Arduino and Circuit-/Micropython. The Python variants are not yet available at the release date 01.03.2024. The Arduino platform STM32duino [sta1] has contained the extension for the STM32WBA family in the main branch since 29/02/2024. However, the planned release 2.8 is not yet available at the time of publication. According to the instructions [sta2], however, a Git branch can also be used for development.

In order to be able to compare the implementation with Circuit- and Micropython, the MultiSensor module was used with the Nucleo-F767 platform, which has been available for some time. This platform can be used in all three environments.

A test shield from a previous project is used to connect the MultiSensor module to the Nucleo-F7676. Only the QWIIC (I2C) connector is used. To connect to the Nucleo-WBA52, a shield has been developed that only provides a QWIIC connector. Due to component availability, the design only includes a short socket strip on which the 3.3V connection is missing. Therefore a 3.3V LDO is used on the board. As this board could not be finalised at the time of publication, a standard QWIIC cable with male jumpers is used to connect the Nucleo board with the MultiSensor module.

Both adapter boards were designed with eagle. The CAD data is available in the project repository at Github (link under software used)

## Sensor hardware

The MultiSensor module contains various sensors for measuring environmental parameters. All sensors are connected to the MCU via an I2C bus. The sensors were selected based on the availability of the ICs and the availability of open source drivers for the development environments analysed. The I2C bus can be used in two ways: on a pin header and via dual QUIIC sockets. As there is an overlap between the I2C addresses of the APDS9960 and AS7341 light sensors, two I2C buses are used. However, both buses are connected by default via solder jumpers. The two solder jumpers only need to be disconnected if both light sensors are to be used. In this case, the APDS9960 is no longer available at the QWIIC sockets, but only via the pin headers.

The only special mechanical feature of the board layout is the thermal decoupling of the two temperature sensors BME680 and HTU21 on the left-hand side of the board.

![[images/pic2.jpg]]

- Sensor 1: CO2 sensor SCD4x

The SCD-4x CO2 sensors from Sensirion have a measuring range of 400-5000 ppm with different accuracies between ±40 and ±75ppm, depending on the exact type. Versions 40, 41 and 42 are pin and software compatible. As the sensor is based on a photoacoustic measuring principle, it is relatively small and energy-saving compared to older sensors based on the non-dispersive infrared sensor principle [MP]. The ambient air in the measuring chamber is irradiated with a defined light pulse (wavelength approx. 4.3µm). The gas molecules absorb the light. The result is an increase in pressure in the measuring cell. The increase in pressure generates sound waves that can be picked up by a microphone.

For good indoor air quality, the CO2 value should be below 800ppm. The maximum guideline value for indoor spaces is 1500ppm [CIK]. At this value, signs of fatigue can already occur.

- Sensor 2: SGP4x

The SGP-40 and 41 indoor air quality sensors from Sensirion measure volatile organic compounds (VOCs) such as sweat, urea, paint and cleaning agents. The 41 model can also detect nitrogen oxides (NOx). The sensor is a chemical metal oxide semiconductor gas sensor. The surface of the sensor that comes into contact with the gas to be detected is coated with a metal oxide. This changes its resistance, which can be read out as a raw value.

The raw value of the sensor is mapped by a filter function to a measuring range of 0-500, the IAQ index. A value of 100 corresponds to uncontaminated room air.

- Sensor 3: TSL2591 light sensor

The measuring principle of the TSL2591 light sensor from ams is based on two light-to-digital converters that convert the incoming light intensity into electrical signals using photodiodes. The light irradiation changes the electrical voltage measured by the photodiode or the current flowing through the diode. The sensor's two photodiodes measure the part of the light visible to humans and the infrared spectrum in two channels.

- Sensor 4: Temperature, humidity, pressure and gas sensor BME680

The BME680 combination sensor from Bosch is a MEMS sensor that measures the ambient air parameters of temperature, humidity and air pressure. A gas sensor is also included.

- Sensor 5: APDS9960 light sensor

Like sensor 3, the APDS9960 light sensor from Avago can measure light intensity. Colour measurement is also possible. However, in this project the sensor is used for contactless operation, as it can also be used as a proximity sensor and for gesture recognition such as UP-DOWN-RIGHT-LEFT.

- Sensors 6:

The photo shows an assembly variant in which other sensors have not been fitted. The HTU21 is a sensor for temperature and humidity comparable to the BME680. Ambient light measurement can be extended with the two light sensors LTR390 and AS7341. The LTR390 can measure ultraviolet light in two spectra. The AS7341 is sensitive in the visible and near IR range, similar to the TSL2561, but is divided into 11 channels so that colour temperatures can also be determined.

The circuit and layout were designed using eagle. The respective data sheet information and open hardware layouts were generally used from the Adafruit Github account [ada]. The CAD data is available in the project repository at Github (link under Software used)

The CAD data is available on the car's Github account. The hardware is fully tested, including the sensors missing in the figure above.

![[images/pic3.jpg]]
![[images/pic4.jpg]]

# Software

- Variant 1: Nucleo-F767 and Micropython

The implementation with Micropython is only a proof of concept due to limitations of the current Micropython implementation. Usable libraries exist for the BME680, HTU21, SGP40 and SCP4x sensors. The Ethernet interface is directly supported by Micropython. A connection to Thingsboard as an IOT cloud is possible.

As expected, the implementation under Micropython is very quick and easy. The firmware can be flashed with the onboard ST-Link. Utilising existing libraries, the sensors are ready for operation in a short time.

However, the division of the flash has proved to be a significant disadvantage. With the default setting. Of the 2Mbyte flash, only around 80kByte are available for Python code. Most of the rest is reserved for the firmware, although this amount is far from being required.

- Variant 2: Nucleo-F767 and Circuitpython

The statements made about variant 1 also apply to variant 2. As essentially only sensors that are also sold by Adafruit are used, the software support is even simpler than under Micropython. However, the same restriction also applies to Circuitpython: the flash size for the Python sources is too small. Another disadvantage is the lack of network support.

### Summary of variants 1 and 2:

If it were not for the limitation of the flash size, Micro- and Circuitpython could be very good development environments for the use of the MultiSensor. The lack of Ethernet support in Circuitpython is an exclusion criterion if a cloud connection is required.

The MultiSensor, including a cloud connection, could be fully implemented with another MCU platform using the Circuitpyhon environment.

Porting for the NUCLEO-WBA52 platform has not yet taken place, but it can be assumed that this platform can also be used very well under Python. The disadvantage of the flash size is solvable, as there is sufficient unused flash memory available.

- Variant 3: Nucleo-F767 and STM32duino

As the port for the STM32WBAxx family was not yet available at the start of the project, an initial implementation was carried out on the Nucleo-F767. Available Arduino libraries were used for the sensors. With a few minor adjustments to these libraries, all sensors could be used.

- Variant 4: Nucleo-WBA52 and STM32duino

The application developed for variant 3 could be used directly for variant 4. It was only necessary to adapt the I2C pins.

### Summary of variants 3 and 4:

As the Arduino environment has existed for a very long time, there are many examples, drivers etc. available. Not everything is immediately usable. But changes are usually limited. Due to the unavoidable compile-link-flash cycle, development is significantly less agile than with a Python variant. However, the usage is also possible with an acceptable amount of effort.

The sources of the implementations are available in the project repository at Github (link under Software used).

# Literature:

[ad] Adafruit Industries, LLC. url: www.adafruit.com

[cik] CIK Solutions. Kohlenstoffdioxid in Räumen messen und überwachen. url: https://www.cik-solutions.com/anwendungen/co2-im-innenraum/

[mp] MIROSLAV PISKOR. SCD4x – der kleinste CO2-Sensor. Hrsg. von SOS electronic. 2021-04-15. url: https://www.soselectronic.com/de/articles/sensirion/scd4x-der-kleinste-co2-sensor-2559

[sta1] STM32duino project repository. url: https://github.com/stm32duino

[sta2] STM32duino git repo integratio into Arduino IDE. url: https://github.com/stm32duino/Arduino_Core_STM32/wiki/Using-git-repository

# Used Software

STM32duino

https://github.com/stm32duino/Arduino_Core_STM32/archive/a5135d476018c1d1ea7b33afe86ae29aa416cbf7.zip

Circuitpython

https://downloads.circuitpython.org/bin/nucleo_f767zi/de_DE/adafruit-circuitpython-nucleo_f767zi-de_DE-8.2.10.bin

Micropython

https://micropython.org/resources/firmware/NUCLEO_F767ZI-20240222-v1.22.2.hex

Project sourcen

https://github.com/ATM-HSW/STM32WirelessInnovationDesignContest