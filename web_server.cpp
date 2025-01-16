#include "web_server.h"
#include "servo_control.h"

WebServer server(80);

void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/verifyOTP", HTTP_POST, handleVerifyOTP);

  server.begin();
}

void serverHandle() {
  server.handleClient();
}

void handleRoot() {
  String otpHtml = "<html><body><h2>Password</h2>";
  otpHtml += "<form action='/verifyOTP' method='post'>";
  otpHtml += "Enter your Pass: <input type='password' name='otp' required><br>";
  otpHtml += "<input type='submit' value='Verify OTP'>";
  otpHtml += "</form></body></html>";
  server.send(200, "text/html", otpHtml);
}

void handleVerifyOTP() {
  String enteredOTP = server.arg("otp");

  if (enteredOTP == "0000") {
    unlock();
  }
}
