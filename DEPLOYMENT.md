# Clocklet deployment procedure

1) Plug in, then insert battery
2) Burn efuses (including serial number)
3) Apply profile to CP2012N (with Simplicity Studio)
4) Flash the firmware

## Clockbrain efuse map

EFUSE block 3:
0        1        2        3        4        5        6        7        << words
0        4        8        12       16       20       24       28       << bytes
0        32       64       96       128      160      192      224      << bits
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 << hex
reserved xxxx                                         ^ hw     ^ serial

So that's block 6 for the hardware revision (uint16) and case colour (uint16)
And block 7 for the serial number (uint32)

## Writing the efuse

Adjust the serial number in the `efuse.bin` file - remember it's LITTLE ENDIAN.

hwrev    serial
V        V
05000300 08000000
    ^
    case colour

And burn it!

~/esp/esp-idf-3.2.2/components/esptool_py/esptool/espefuse.py --port /dev/tty.SLAB_USBtoUART burn_block_data --offset 24 BLK3 efuse.bin

(use --force-write-always if necessary)

Case colours
0 unset raw hardware
1 fake wood
2 translucent
3 rainbow blue-pink
4 white
5 galaxy black (default)