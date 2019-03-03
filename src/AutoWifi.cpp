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
bool AutoWifi::startWifi(uint32_t timeLimitMsec)
{
    if (isProvisioned())
    {
        reconnect();
        return true;
    }

    //Init WiFi as Station, start SmartConfig
    WiFi.mode(WIFI_AP_STA);

    uint32_t start = millis(); // Wait a bit in hopes we can connect

#ifdef FACTORY_WIFI_SSID
    // Serial.printf("First trying factory wifi\n");
    WiFi.begin(TOSTRING(FACTORY_WIFI_SSID), TOSTRING(FACTORY_WIFI_PASSWORD));
    while (millis() - start < 5000)
    {
        delay(100);
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.printf("Using factory wifi!\n");
            ssid = WiFi.SSID(); // this only updates the inram copies for this session (which is what we want)
            password = WiFi.psk();
            return true;
        }
    }
    WiFi.disconnect();
#endif

    bool success = false;
    while (!success && millis() - start < timeLimitMsec)
    {
        Serial.println("Starting SmartConfig.");
        WiFi.beginSmartConfig();

        //Wait for SmartConfig packet from mobile
        Serial.println("Waiting for SmartConfig.");
        while (!WiFi.smartConfigDone() && millis() - start < timeLimitMsec)
        {
            delay(500);
            Serial.print(".");
        }

        ssid = WiFi.SSID();
        password = WiFi.psk();
        Serial.printf("SmartConfig ssid: %s\n", ssid.c_str());

        Serial.println("Waiting for WiFi to connect");
        while (WiFi.status() != WL_CONNECTED && millis() - start < timeLimitMsec)
        {
            delay(500);
            Serial.print(".");
        }
        success = WiFi.status() == WL_CONNECTED;
    }

    if (success)
    {
        Serial.println("Success, saving preferences");

        // We _only_ save our wifi prefs if we can connect to the wifi.  Just in case the client sent a shit setting
        Preferences p;
        p.begin("AutoWifi", false);

        p.putString("ssid", ssid);
        p.putString("password", password);

        // Close the Preferences
        p.end();
    }

    return success;
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
