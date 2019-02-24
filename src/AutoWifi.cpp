#include <AutoWifi.h>
#include <Preferences.h>
#include <WiFi.h>

/**
   Copyright 2019 S. Kevin Hester-Chow, kevinh@geeksville.com, MIT License
   https://github.com/geeksville/AutoWifi
 */

AutoWifi::AutoWifi() : readPrefs(false)
{
}

/**
   If true we already have wifi settings, if false, you should tell the user we
   will soon be in SmartConfig mode and then call startWifi().
 */
bool AutoWifi::isProvisioned()
{
    // we don't read our prefs at constructor time.
    if (!readPrefs)
    {
        Preferences p;
        p.begin("AutoWifi", false);

        ssid = p.getString("ssid");
        password = p.getString("password");

        // Close the Preferences
        p.end();
        readPrefs = true;
    }
    return ssid.length() > 0;
}

void AutoWifi::reconnect()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.printf("Connecting to wifi, SSID %s\n", ssid.c_str());
        if (password.length() > 0)
            WiFi.begin(ssid.c_str(), password.c_str());
        else
            WiFi.begin(ssid.c_str());
    }
}

// nasty magic to get preprocessor macros as string
#define STRINGIFY(m) #m
#define TOSTRING(x) STRINGIFY(x)

/**
   If wifi has already been proivisoned in EEPROM, setup the wifi and continue boot.

   Otherwise stay in smart config mode until the user has provisioned wifi.
 */
void AutoWifi::startWifi()
{
    if (isProvisioned())
    {
        reconnect();
    }
    else
    {
        //Init WiFi as Station, start SmartConfig
        WiFi.mode(WIFI_AP_STA);

#ifdef FACTORY_WIFI_SSID
        // Serial.printf("First trying factory wifi\n");
        WiFi.begin(TOSTRING(FACTORY_WIFI_SSID), TOSTRING(FACTORY_WIFI_PASSWORD));
        uint32_t start = millis(); // Wait a bit in hopes we can connect
        while (millis() - start < 5000)
        {
            delay(100);
            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.printf("Using factory wifi!\n");
                return;
            }
        }
        WiFi.disconnect();
#endif

        Serial.println("Starting SmartConfig.");
        WiFi.beginSmartConfig();

        //Wait for SmartConfig packet from mobile
        Serial.println("Waiting for SmartConfig.");
        while (!WiFi.smartConfigDone())
        {
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
void AutoWifi::resetProvisioning()
{
    Preferences p;
    p.begin("AutoWifi", false);

    // Remove all preferences under the opened namespace
    p.clear();
    ssid = ""; // mark invalid
    password = "";

    // Close the Preferences
    p.end();
}
