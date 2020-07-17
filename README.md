# Clocklet

See also [clocklet.co.uk](https://clocklet.co.uk)

## Overview

This project is the firmware for clocks made by @samscam including:

### Clocklet
* 17x5 SK9822 LED Matrix display
* Timekeeping: ESP32 (DS3231 as backup)
* Synchronisation: NTP
* Microcontroller: ESP32 Wrover on custom "Clockbrain" board

### Old Clocklet
Earlier prototypes used RGBDigit (WS2812B) 7 segment display and Adafruit Feather Huzzah32

### Mrs Clockety
* Display: Waveshare epaper 2.9inch https://www.waveshare.com/2.9inch-e-paper-module.htm
* Timekeeping: GPS
* Synchronisation: GPS
* Power management to run from a lipo battery
* Microcontroller: Adafruit Feather Huzzah32

It's all a work in progress.

## Main Features

* Time
* Weather
* Utterances
* Interesing displays and animation
* Ambient light sensing
* Power management (Mrs Clockety)

## Building, configuration, uploading firmware

* Clone the repo including submodules
* Install platformio and your editor of choice.
* Open up the project
* Pick the *correct environment* for the device you want to build for (by setting it in the first line of `platformio.ini` or by invoking tasks for the specific environment in the editor/cli)
* Adjust the default settings in `settings.h` including API keys
* Plug the device in to your machine over USB
* Build and upload (platformio should download the dependencies if it hasn't done so already)

