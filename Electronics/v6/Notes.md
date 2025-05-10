# V6 design notes

## USB-C Socket

On the v5 I had a sticky-out socket (in the middle of the board)
For this one do we want to carry on with that, and have it exit at the rear, (standing paste) or come out at the side (horizontal attach, which would be simpler??)

There are bazillions in the JLC catalogue but we are talking
- 16 pin
- good enough rating for current (need to work out hypothetical max limit)

## ESD protection

Doing this right is more complex than I had thought.

We need over-voltage protection and ESD protection.
on Vbus
CC lines
D+/- lines

Options: 

### The semtech way:
https://www.semtech.com/uploads/design-support/TVS_App_Notes-SI21-03-ESD_Protection_of_USB_Type-C_Interfaces.pdf

Semtech TDS2221PW for vBUS
µClamp2411ZA for CC
RClamp4021ZA for D+/-

Or they do this thing
https://estore.st.com/en/tcpp01-m12-cpn.html


### The TI way:
https://www.ti.com/lit/ta/ssztal6/ssztal6.pdf

TPD6S300
does 6 channels CC, SBU, D+/-
or TPD8S300 for 8 channels
Still need something for vbus
https://www.ti.com/lit/ds/slvsdk3c/slvsdk3c.pdf?ts=1744796360041

TPD4S480
4 channels, newer, 48v compliant
https://www.ti.com/lit/ds/symlink/tpd4s480.pdf

ESD122 - if we wanted to do the high speed lines (but we don't)
https://www.ti.com/lit/ds/symlink/esd122.pdf


## USB-PD controller

TPS25730
https://www.ti.com/lit/ds/symlink/tps25730.pdf

Has a 3v3 LDO built in for output BUT that's limited to 0.1A

TPS25751 has built in esd protection... and moisure detection
https://www.ti.com/lit/ds/symlink/tps25751.pdf

TPS26750 doesn't have esd - says you should pair with TPD4S480
https://www.ti.com/product/TPS26750

## LDO

These both coming off the PPHV line of the PD controller...

3.3v
then we need an LDO for the 3v3 supply for the MCU
old one was AP2112K-3.3
JLC have AMS1117 (which is rated for 1A)

5v
Presuming that we are sticking with 5v LEDs...

Possibly a buck?

Or is there a dual channel power regulator thing? One for 5v and one for 3.3v?

## MUX

It's probably unnecessary but we could put a mux on the D+- lines. Or just join them together.

## MCU

Espressiv ESP32-S3
https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf

Sensible plan is to use the "mini" one with built-in antenna.
Which does have placement implications.
It's that or go there with doing RF and have an antenna somewhere.
Current rating recommendation 500mA.


## LEDs

current - 5*17 matrix = 85 LEDs

Currently using SK9822
http://www.normandled.com/upload/201909/SK9822%20LED%20Datasheet.pdf
Those are max 20ma each = 1.7A

Which puts our total max power budget at roughly 2.4A * 5V = 12W
(but we never actually go that high, probably)

As we are getting PCBA this time... Possibly do 2020 ones?

2020s seem to work out more like 18ma


HD107
HD108 - 16bit - 20ma in the 5050 package
https://www.rose-lighting.com/wp-content/uploads/sites/53/2019/11/HD108-led.pdf
(rose lighting datasheets are a mess)

WS2816 (recently supported in fastled) - is a 1-wire thing but 16-bit.
Better availability... but not sure I like it and would prefer the APA102 style spi interface.
SK9822 are readily available.

BIG QUESTION:
Single board with everything on one side.
Double sided.
Sandwich

