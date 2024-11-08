#include "wifi_config.h"

const char *ssid = "iitk-sec";
String ip;
String url = "https://eclubdoorlock.netlify.app/set?url=http://";

void setupWiFi() {
  int counter = 0;
  Serial.print("Connecting to network: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);

  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));

  esp_wifi_sta_wpa2_ent_enable();

  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;

    if (counter >= 120) {  // Wait for 60 seconds (adjust as needed)
      Serial.println("\nConnection timed out. Restarting.");
      ESP.restart();
      while (1) { delay(1); }
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address set: ");
  Serial.println(WiFi.localIP());
  ip = WiFi.localIP().toString();

  UpdateRedirectPage();
}

void checkIPChange() {
  if (WiFi.localIP().toString() != ip) {
    String newIP = "The IP has been reset to ";
    newIP += WiFi.localIP().toString();
    newIP += " for security reasons.";
    ip = WiFi.localIP().toString();

    UpdateRedirectPage();
  }
}

void UpdateRedirectPage() {
  if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(url + ip);
      
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}