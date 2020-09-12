# Clocklet

See also [clocklet.co.uk](https://clocklet.co.uk)

## Overview

This project is the firmware and client apps for clocks made by @samscam including:

### Clocklet
* 17x5 SK9822 LED Matrix display
* Timekeeping: ESP32 (DS3231 as backup)
* Synchronisation: NTP
* Microcontroller: ESP32 Wrover on custom "Clockbrain" board

### Old Clocklet / Clocky
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

## Firmware

To build the the firmware:

* Clone the repo **including submodules**
* Install platformio and your editor of choice - I'm using vscode.
* Open up the firmware from the `Embedded` directory
* Pick the *correct environment* for the device you want to build for (by setting it in the first line of `platformio.ini` or by invoking tasks for the specific environment in the editor/cli) this will probably be `clockbrain`
* Plug the device in to your machine over USB
* Check that the device is available to your system... on MacOS it's normally `/dev/cu.SLAB_USBtoUART`
* Build and upload (platformio should download the dependencies if it hasn't done so already)
* Note that API keys are encrypted with gitcrypt

### Seeing the logs (without installing anything)

You can connect to the clocklet's serial port with various tools (like screen, or putty) and see the Clocklet logging.

The important thing to note is that the baud rate of the serial connection is `115200`.

`screen /dev/cu.SLAB_USBtoUART 115200`


### Manually updating firmware (with some installing)

You'll need to install `esptool`
On the mac this can be done with homebrew: `brew install esptool`

Download the bin file of the firmware image you want from the releases section on here.

Erase the flash (including settings)
`esptool.py -b 115200 -p /dev/cu.SLAB_USBtoUART erase_region 0x9000 0xFF7000`

Flash the new firmware image:

`esptool.py -b 115200 -p /dev/cu.SLAB_USBtoUART write_flash 0x10000 firmware.bin`

While we are here you can also use esptool to just erase settings (equivalent of doing a factory reset from the app):
`esptool.py -b 115200 -p /dev/cu.SLAB_USBtoUART erase_region 0x9000 0x5000`


## Client App (iOS)

*Requires Xcode 12 (beta)*
To build, open via the workspace rather than the Xcode Project.

Contains an experimental library for gluing CoreBluetooth to Combine and SwiftUI via PropertyWrappers called "CombineBluetooth". This may get spun out into a separate project in the future - but for ease of development right now it's in here.


## To follow

* Android client
* Enclosure design (3d printable)
* Hardware design

## License

This is distributed under the MIT License

    MIT License

    Copyright (c) 2017-2020 Sam Easterby-Smith

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
