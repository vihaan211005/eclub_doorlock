#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#if __has_include("esp_eap_client.h")
#include "esp_eap_client.h"
#else
#include "esp_wpa2.h"
#endif

#define EAP_ANONYMOUS_IDENTITY "svihaan23"
#define EAP_IDENTITY "svihaan23"
#define EAP_PASSWORD "VIhaan23!$pass"

extern String ip;

void setupWiFi();
void checkIPChange();
void UpdateRedirectPage();

#endif