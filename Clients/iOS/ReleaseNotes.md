# Clocklet iOS Release notes

## v1.0 (92)

This goes with Clocklet firmware v1.0.1

### NEW USERS HELLO!

Plug in your clock, run the app, set up network and location. Happy clock!

### NEW STUFF IN THIS VERSION

* Changing locations
You can now change the location of the Clocklet on the new location settings screen. There are some canned examples or you can set it to your real location. Changing location will also trigger a weather update - though if you change locations repeatedly you will likely end up with the weather for a previous place.

* International time zone and DST support
The clock's time should reflect the time in the chosen location. Even for places with crazy timezones.

* Bluetooth status improvements
On your phone, if bluetooth is switched off or not authorised you should see a helpful overlay telling you to do something about it.
The app will now correctly report when pairing data is messed up - you can (if you really want to) test this by performing a factory reset on the clock and then trying to reconnect. To clear the error, remove the clocklet from your phone's bluetooth settings and try connecting again. No need to kill the app.

* Various layout improvements in the app
* Fixed occasional jerkyness on the Clocklet's display (by running network tasks on the other core)

### MIGRATIONS FROM EARLIER VERSIONS

After installing the app, check your Clocklet's firmware version is v1.0.1 in the technical stuff section. Rebooting will trigger an update if it hasn't already done it.

Background or quit the app while the clock is doing the firmware update - otherwise the update will fail.

After performing a firmware update, or otherwise if settings are missing/messed up in the app, you should switch bluetooth on your phone off and on again to clear the bluetooth cache.

After migrating, your phone will report the Clock's location as "Nowhere" - just change the location to sort it out.

### ANNOYING BUGS

I've had a few times where the Clocklet fails to advertise itself properly, thus either not showing up in the app or looking like a circuit board. Turning the clock off and on again should fix it.

Have seen some occasional crashes on the clocklet (shortly after startup or after multiple location changes) but they also seem to sort themselves out. Will carry on trying to track them down.