#include "web_server.h"
#include "member_management.h"
#include "email_sender.h"
#include "lcd_display.h"
#include "servo_control.h"
#include "fingerprint.h"

WebServer server(80);

String generatedOTP;

String generateOTP() {
  return String(random(100000, 999999));
}

void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);

  server.on("/register", HTTP_GET, handleFormPage);
  server.on("/register", HTTP_POST, handleMemberSubmit);

  server.on("/unlock", HTTP_GET, handleUnlockPage);
  server.on("/unlock", HTTP_POST, handleUnlock);

  server.on("/changepassword", HTTP_GET, handleChangepasspage);
  server.on("/changepassword", HTTP_POST, handleChangepass);

  server.on("/deleteuser", HTTP_GET, handleDeleteuserPage);
  server.on("/deleteuser", HTTP_POST, handleDeleteuser);

  server.on("/otp", HTTP_GET, handleOTPVerification);
  server.on("/otpverification", HTTP_POST, handleOTPVerificationPage);
  server.on("/verifyOTP", HTTP_POST, handleVerifyOTP);

  server.begin();
}

void serverHandle() {
  server.handleClient();
}

void handleRoot() {

  String html = "<html><head><meta http-equiv='refresh' content='1'></head><body>";  // Refresh every 10 seconds
  html += "<h1>Member Data</h1>";
  html += "<table border='1'><tr><th>Name</th><th>ID</th><th>Inside Status</th></tr>";

  for (const Member& m : members) {
    html += "<tr><td>" + m.name + "</td><td>" + String(m.id) + "</td><td>" + (m.inside_status ? "Inside" : "Outside") + "</td></tr>";
  }
  html += "</table>";
  html += "<br><a href='/otp?action=register'>Register</a>";
  html += "<br><a href='/otp?action=unlock'>Unlock</a>";
  html += "<br><a href='/otp?action=changepassword'>Change Password</a>";
  html += "<br><a href='/deleteuser'>Delete User</a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}


void handleFormPage() {
  String mailID = server.arg("mailID");

  String adminOTP = generateOTP();
  Serial.println("admin otp " + adminOTP);
  String content = "OTP to authorise " + mailID + "to enter the World of Electronics is " + adminOTP + ".";

  sendMail((String)AUTHOR_EMAIL, "One Time Password", content);

  String formHtml = "<html><body><h2>Registration Form</h2>";
  formHtml += "<form action='/register' method='post'>";
  formHtml += "Name: <input type='text' name='name'><br>";
  formHtml += "ID: <input type='number' name='id'><br>";
  formHtml += "Create Password: <input type='password' name='password'><br>";  // Member password
  formHtml += "OTP: <input type='password' name='otp'><br>";                   // Admin password
  formHtml += "<input type='hidden' name='mailID' value='" + mailID + "'>";
  formHtml += "<input type='hidden' name='adminOTP' value='" + adminOTP + "'>";
  formHtml += "<input type='submit' value='Register'>";
  formHtml += "</form></body></html>";
  server.send(200, "text/html", formHtml);
}





void handleMemberSubmit() {
  String memberName = server.arg("name");
  int memberID = server.arg("id").toInt();
  String memberPassword = server.arg("password");
  String otp = server.arg("otp");
  String mailID = server.arg("mailID");
  String adminOTP = server.arg("adminOTP");

  if (adminOTP == otp) {
    getFingerprintEnroll(memberID, memberName, memberPassword, mailID);
    String content = "Dear " + memberName + ",you have been successfully authorised to access the World of Electronics.";
    sendMail(mailID, "Registration Complete", content);
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  }

  else {
    displayMessage("invalid OTP", 0);
    String content = "Dear " + memberName + ", the OTP entered was incorrect or the iD is already taken. Please try again.";
    sendMail(mailID, "Registration Failed", content);
    delay(2000);
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "Invalid admin password.");
  }
}


void handleUnlockPage() {

  String mailID = server.arg("mailID");
  String unlockHtml = "<html><body><h2>Unlock Door</h2>";
  unlockHtml += "<form action='/unlock' method='post'>";
  unlockHtml += "ID: <input type='number' name='id' required><br>";
  unlockHtml += "Password: <input type='password' name='password' required><br>";
  unlockHtml += "<input type='hidden' name='mailID' value='" + mailID + "'>";
  unlockHtml += "<input type='submit' value='Unlock'>";
  unlockHtml += "</form></body></html>";
  server.send(200, "text/html", unlockHtml);
}
void handleUnlock() {
  String id = server.arg("id");
  String enteredPassword = server.arg("password");
  String mailID = server.arg("mailID");
  int memberId = id.toInt();

  for (const Member& m : members) {
    if (m.id == memberId && m.password == enteredPassword && m.mailID == mailID) {
      Member* foundMember = getMemberByID(m.id);
      foundMember->inside_status = !foundMember->inside_status;
      String content = foundMember->name + " has accessed the Electronics Club through the web page.";
      sendMail((String)AUTHOR_EMAIL, "Club Opened", content);
      unlock();
      displayMessage("door unlocked", 3000);
      server.sendHeader("Location", "/", true);
      server.send(302, "text/plain", "");
      return;
    }
  }
  displayMessage("invalid creds", 3000);
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}


void handleChangepasspage() {
  String mailID = server.arg("mailID");
  String changepassHtml = "<html><body><h2>Change Password</h2>";
  changepassHtml += "<form action='/changepassword' method='post'>";
  changepassHtml += "Name: <input type='text' name='name'><br>";
  changepassHtml += "ID: <input type='number' name='id' required><br>";
  changepassHtml += "New Password: <input type='password' name='newpass' required><br>";
  changepassHtml += "<input type='hidden' name='mailID' value='" + mailID + "'>";
  changepassHtml += "<input type='submit' value='Change Password'>";
  changepassHtml += "</form></body></html>";

  server.send(200, "text/html", changepassHtml);
}
void handleChangepass() {
  uint8_t id = server.arg("id").toInt();
  String newpass = server.arg("newpass");
  String name = server.arg("name");
  String mailID = server.arg("mailID");

  Member* foundMember = getMemberByID(id);

  if (foundMember->name == name && foundMember->mailID == mailID) {
    foundMember->password = newpass;

    displayMessage("pass changed", 2000);
  } else {
    displayMessage("invalid creds", 2000);
  }
  delay(3000);
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}



void handleDeleteuserPage() {
  String adminOTP = generateOTP();
  Serial.println("admin otp " + adminOTP);
  String content = "OTP for the current deratification session is " + adminOTP + ", take this decision wisely.";
  sendMail((String)AUTHOR_EMAIL, "Deratification OTP", content);

  String deleteuserHtml = "<html><body><h2>Delete User</h2>";
  deleteuserHtml += "<form action='/deleteuser' method='post'>";
  deleteuserHtml += "Name: <input type='text' name='name'><br>";
  deleteuserHtml += "ID: <input type='number' name='id' required><br>";
  deleteuserHtml += "OTP: <input type='password' name='otp' required><br>";
  deleteuserHtml += "<input type='hidden' name='adminOTP' value='" + adminOTP + "'>";
  deleteuserHtml += "<input type='submit' value='Delete User'>";
  deleteuserHtml += "</form></body></html>";
  server.send(200, "text/html", deleteuserHtml);
}
void handleDeleteuser() {
  uint8_t iD = server.arg("id").toInt();
  String name = server.arg("name");
  String adminOTP = server.arg("adminOTP");
  String otp = server.arg("otp");

  Member* foundMember = getMemberByID(iD);
  if (foundMember->name == name && foundMember->name == name && otp == adminOTP) {
    String content = "Dear " + foundMember->name + ",you have been successfully deratified and barred from entering the Electroncis Club.";
    sendMail(foundMember->mailID, "Deratified", content);
    members.erase(members.begin() + memberLocation(iD));
    finger.deleteModel(iD);
    saveToCSVFile();

    displayMessage("deratified", 2000);

  }

  else {
    String content = "Dear " + foundMember->name + ", you were just on the brink of getting deratified from the Electronics Club.";
    sendMail(foundMember->mailID, "Deratification Failed", content);
    displayMessage("invalid creds", 2000);
  }
  delay(3000);
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}


void handleOTPVerification() {
  String action = server.arg("action");
  String otpHtml = "<html><body><h2>OTP Verification</h2>";
  otpHtml += "<form action='/otpverification' method='post'>";
  otpHtml += "Enter your Mail ID: <input type='text' name='mailID'><br>";
  otpHtml += "<input type='submit' value='Send OTP'>";
  otpHtml += "<input type='hidden' name='action' value='" + action + "'>";
  otpHtml += "</form></body></html>";
  server.send(200, "text/html", otpHtml);
}
void handleOTPVerificationPage() {
  String action = server.arg("action");
  String mailID = server.arg("mailID");
  String otpHtml = "<html><body><h2>OTP Verification</h2>";
  otpHtml += "<form action='/verifyOTP' method='post'>";
  otpHtml += "Enter your OTP: <input type='password' name='otp' required><br>";
  otpHtml += "<input type='hidden' name='action' value='" + action + "'>";
  otpHtml += "<input type='hidden' name='mailID' value='" + mailID + "'>";
  otpHtml += "<input type='submit' value='Verify OTP'>";
  otpHtml += "</form></body></html>";
  generatedOTP = generateOTP();
  Serial.println(generatedOTP);
  String content = "Here is your One Time Password " + generatedOTP + " for your chosen action-" + action + ".";
  sendMail(mailID, "One Time Password", content);
  server.send(200, "text/html", otpHtml);
}

void handleVerifyOTP() {
  String enteredOTP = server.arg("otp");
  String action = server.arg("action");
  String mailID = server.arg("mailID");
  if (enteredOTP == generatedOTP) {
    server.sendHeader("Location", "/" + action + "?mailID=" + mailID, true);
    server.send(302, "text/plain", "");
  } else {

    String content = "The OTP you have provided is incorrect, Please try again.";
    sendMail(mailID, "Incorrect OTP", content);

    server.sendHeader("Location", "/?error=Invalid OTP", true);
    server.send(302, "text/plain", "");
  }
}