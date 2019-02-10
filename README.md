# AutoWifi

Making ESP32 wifi setup trivial with Android/iOS helper apps.  5 lines of code and no more hardcoded keys.

# Usage

I poked around on the internet for documentation on the usage of the ESP32 SmartConfig
wifi setup flow.  I found some tutorials, but it seems painful that people keep having
to reinvent the standard boilerplate of: talking to the app, saving prefs in EEPROM,
letting the user 'factory reset' as needed etc...  So I made this simple little lib.

You can add it to your ESP32 project trivially - thus removing the barbaric practice of
telling 'users' to edit a .cpp file to insert their SSID and password.  Instead users
can run an Android/iOS app and have a polished process for starting your app on new hardware.
You can even just give them bins and they can skip compilation or you can use this code to
ship your finished product.

This is all enabled by the SmartConfig code in the ESP32 libraries.  

## Supported Android and iOS configuration applications

Espressif provides source code to (Android)[https://github.com/EspressifApp/EsptouchForAndroid] and (iOS apps)[https://github.com/EspressifApp/EsptouchForIOS] you can configure for your product.
However, for early development/hobbyists some kind souls out on the internet have already
built suitable apps and placed them in the appstore.  

* For Android I recommend (this one)[https://play.google.com/store/apps/details?id=com.iotmaker&rdid=com.iotmaker].
* For iOS I've been told (this one)[https://itunes.apple.com/us/app/espressif-esptouch/id1071176700?mt=8] works,
but I have not tested it.  Please add a github issue if it works/doesn't for you.

# Installation

If you are using PlatformIO you can add this library to your project with "pio lib install AutoWifi".

Just replace your existing "WiFi.begin(ssid, password)" call with:
```
  #include <AutoWifi.h>
  AutoWifi a;
  a.startWifi();
```

That's it!  If your board is unconfigured SmartConfig mode will automatically start.  
But if the config has been completed in the past it will just join that wifi network.

If you want to show the user the device is in SmartConfig mode call isProvisioned() before calling startWifi().
If you want to reset the provisioning (i.e. 'factory reset') call resetProvisioning().  Wifi settings are stored
in a portion of the 'EEPROM' preferences.

# Reporting issues

Issues and pull requests are happily accepted, please use github.

Copyright 2019 S. Kevin Hester-Chow, kevinh@geeksville.com, MIT License
https://github.com/geeksville/AutoWifi
