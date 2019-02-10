#include <AutoWifi.h>
#include <Preferences.h>
#include <WiFi.h>

/**
   Copyright 2019 S. Kevin Hester-Chow, kevinh@geeksville.com, MIT License
   https://github.com/geeksville/AutoWifi
 */

AutoWifi::AutoWifi() {
    Preferences p;
    p.begin("AutoWifi", false);

    ssid = p.getString("ssid");
    password = p.getString("password");

    // Close the Preferences
    p.end();
}

/**
   If true we already have wifi settings, if false, you should tell the user we
   will soon be in SmartConfig mode and then call startWifi().
 */
bool AutoWifi::isProvisioned() {
    return ssid.length() > 0;
}

/**
   If wifi has already been proivisoned in EEPROM, setup the wifi and continue boot.

   Otherwise stay in smart config mode until the user has provisioned wifi.
 */
void AutoWifi::startWifi() {
    if(isProvisioned()) {
        Serial.printf("Using saved wifi config, SSID %s\n", ssid.c_str());
        if(password.length() > 0)
            WiFi.begin(ssid.c_str(), password.c_str());
        else
            WiFi.begin(ssid.c_str());
    }
    else {
        //Init WiFi as Station, start SmartConfig
        WiFi.mode(WIFI_AP_STA);
        WiFi.beginSmartConfig();

        //Wait for SmartConfig packet from mobile
        Serial.println("Waiting for SmartConfig.");
        while (!WiFi.smartConfigDone()) {
            delay(500);
            Serial.print(".");
        }

        ssid = WiFi.SSID();
        password = WiFi.psk();
        Serial.printf("SmartConfig ssid: %s\n", ssid.c_str());

        Preferences p;
        p.begin("AutoWifi", false);

        p.putString("ssid", ssid);
        p.putString("password", password);

        // Close the Preferences
        p.end();
    }
}

/**
   Throw away our wifi settings, any future calls to startWifi() will force the
   user to reprovision.

   Call this if the user presses your 'completely reset this device' button.
 */
void AutoWifi::resetProvisioning() {
    Preferences p;
    p.begin("AutoWifi", false);

    // Remove all preferences under the opened namespace
    p.clear();
    ssid = ""; // mark invalid
    password = "";

    // Close the Preferences
    p.end();
}
