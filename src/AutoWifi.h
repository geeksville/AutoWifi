#pragma once

#include <Arduino.h>

/**
   Making ESP32 wifi setup trivial with Android/iOS helper apps.  5 lines of code and no more hardcoded keys.

   Features:
 * Developer can reset wifi prefs to force a clean boot and again prompt user for wifi setup
 * Uses the SmartConfig Android and iOS apps to let the user set wifi provisioning
 * stores wifi SSD and password in 'EEPROM' Preferences
 * If wifi is provision boot proceeds automatically, otherwise it enters SmartConfig mode

   Copyright 2019 S. Kevin Hester-Chow, kevinh@geeksville.com, MIT License
   https://github.com/geeksville/AutoWifi
 */
class AutoWifi
{
  String ssid;
  String password;
  bool readPrefs;

public:
  AutoWifi();

  /**
       If true we already have wifi settings, if false, you should tell the user we
       will soon be in SmartConfig mode and then call startWifi().
     */
  bool isProvisioned();

  /**
       If wifi has already been proivisoned in EEPROM, setup the wifi and continue boot.

       Otherwise stay in smart config mode until the user has provisioned wifi.

       Call this _instead_ of WiFi.begin(...)

       return true if we succeeded in the time we were allowed, or false otherwise
     */
  bool startWifi(uint32_t timeLimitMsec = 5 * 60 * 1000);

  /**
       Throw away our wifi settings, any future calls to startWifi() will force the
       user to reprovision.

       Call this if the user presses your 'completely reset this device' button.
     */
  void resetProvisioning();

  /**
       Call this from your loop() function occasionally.  If we have lost our wifi connection it
       will attemp to reconnect
     */
  void reconnect();

  String getSSID()
  {
    return ssid;
  }
};
