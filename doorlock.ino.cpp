#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <FS.h>
#include <SD.h>
#include <LittleFS.h>
#include <WiFiClientSecure.h>
#if __has_include("esp_eap_client.h")
#include "esp_eap_client.h"
#else
#include "esp_wpa2.h"
#endif
#include <Adafruit_Fingerprint.h>
#define EAP_ANONYMOUS_IDENTITY ""
#define EAP_IDENTITY "" // enter IITK username here eg. "dhruvm22"
#define EAP_PASSWORD "" // enter password here eg. "12345678"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP_Mail_Client.h>

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "doorlock22eclub@gmail.com"
#define AUTHOR_PASSWORD "aorv fsxj uuoq bsfa"

// capacitive keypad
#define SCL 8
#define SDO 9
byte Key;

byte Read_TTP229_Keypad(void)
{
    byte Num;
    byte Key_State = 0;
    for (Num = 1; Num <= 16; Num++)
    {
        digitalWrite(SCL, LOW);
        if (!digitalRead(SDO))
            Key_State = Num;
        digitalWrite(SCL, HIGH);
    }
    return Key_State;
}

// keypad library
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Change to id of cordis
int cordi_ids[] = {1, 2, 3, 4, 5};

SMTPSession smtp;

LiquidCrystal_I2C lcd(0x27, 20, 4);

const char *ssid = "iitk-sec";

WiFiClientSecure client;
int counter = 0;
bool wifiConnected = false;

#include <WebServer.h>
#include <vector>

WebServer server(80);
using namespace std;

class member
{
public:
    String name;
    int id;
    bool inside_status;
    String password;
    String mailiD;

    member(const String &n, int i, bool inside, const String &pass, const String &mail) : name(n), id(i), inside_status(inside), password(pass), mailiD(mail) {}
};

const char *filename = "/data.csv";

vector<member> members;

int pos = 180;
Servo myservo;

HardwareSerial fingerSerial(2); // Serial2 on ESP32 (TX=GPIO17, RX=GPIO16)
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

// const int password = 12345;
bool isPasswordCorrect = false;

IPAddress staticIP(172, 24, 18, 13); // Set your desired static IP address
IPAddress gateway(172, 24, 23, 254); // Set your router's IP address
IPAddress subnet(255, 255, 248, 0);  // Set your subnet mask
IPAddress pdns(172, 31, 1, 130);
IPAddress sdns(172, 31, 1, 1);

String generatedOTP;

// 82 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
String generateOTP();
// 88 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void setup();
// 187 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
member &getMemberByID(int memberID);
// 199 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void sendMail(String recipientEmail, String subject, String content);
// 240 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void smtpCallback(SMTP_Status status);
// 251 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void unlock();
// 265 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
uint8_t getFingerprintEnroll(uint8_t id, String name, String password, String mailiD);
// 399 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void getFingerprintID();
// 468 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
int memberLocation(uint8_t iD);
// 483 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void loop();
// 508 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void saveToCSVFile(const char *path, const vector<member> &data);
// 531 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void loadDataFromCSVFile(const char *path, std::vector<member> &members);
// 564 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleRoot();
// 585 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleFormPage();
// 611 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleMemberSubmit();
// 640 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleUnlockPage();
// 652 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleUnlock();
// 683 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleChangepasspage();
// 697 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleChangepass();
// 728 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleDeleteuserPage();
// 745 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleDeleteuser();
// 781 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleOTPVerification();
// 791 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleOTPVerificationPage();
// 808 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
void handleVerifyOTP();
// 82 "C:\\Users\\dhmit\\OneDrive\\Desktop\\doorlock\\doorlock.ino"
String generateOTP()
{
    return String(random(100000, 999999));
}

void setup()
{
    Serial.begin(115200);

    // capacitive keypad i2c comm
    pinMode(SCL, OUTPUT);
    pinMode(SDO, INPUT);

    myservo.attach(15);
    delay(10);
    Serial.println();
    lcd.init();

    lcd.backlight();
    lcd.clear();

    Serial.print("Connecting to network: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);

    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));

    esp_wifi_sta_wpa2_ent_enable();

    WiFi.begin(ssid);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        counter++;

        if (counter >= 120)
        { // Wait for 60 seconds (adjust as needed)
            Serial.println("\nConnection timed out. Restarting.");
            ESP.restart();
        }
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address set: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(WiFi.localIP());
    delay(2000);

    wifiConnected = true;

    if (!SD.begin())
    {
        Serial.println("SD card initialization failed!");
        return;
    }

    members.clear();
    loadDataFromCSVFile(filename, members);

    smtp.debug(0);
    smtp.callback(smtpCallback);

    fingerSerial.begin(57600); // Initialize HardwareSerial for the fingerprint sensor

    if (finger.verifyPassword())
    {
        Serial.println("Found fingerprint sensor!");
    }
    else
    {
        Serial.println("Did not find fingerprint sensor :(");
        ESP.restart();
        while (1)
        {
            delay(1);
        }
    }
    finger.getParameters();

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

member &getMemberByID(int memberID)
{
    for (member &m : members)
    {
        if (m.id == memberID)
        {
            return m;
        }
    }
    return members[0];
}

void sendMail(String recipientEmail, String subject, String content)
{
    // Declare the Session_Config for user-defined session credentials
    Session_Config config;

    // Set the session config
    config.server.host_name = SMTP_HOST;
    config.server.port = SMTP_PORT;
    config.login.email = AUTHOR_EMAIL;
    config.login.password = AUTHOR_PASSWORD;
    config.login.user_domain = "";

    // Set the NTP config time
    config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
    config.time.gmt_offset = 3;
    config.time.day_light_offset = 0;

    // Declare the message class
    SMTP_Message message;

    // Set the message headers
    message.sender.name = F("ESP32");
    message.sender.email = AUTHOR_EMAIL;
    message.subject = subject.c_str();
    message.addRecipient(F("YOU"), recipientEmail);

    message.text.content = content.c_str();
    message.text.charSet = "us-ascii";
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

    message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
    message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

    // Connect to the server
    if (!smtp.connect(&config))
    {
        return;
    }

    // Start sending Email and close the session
    MailClient.sendMail(&smtp, &message);
}

void smtpCallback(SMTP_Status status)
{
    // Print only the final status of the sent message
    if (status.success())
    {
        Serial.println("Message sent successfully");
    }
    else
    {
        Serial.println("Message send failed");
    }
}

void unlock()
{
    for (pos = 180; pos >= 120; pos -= 1)
    {                       // goes from 180 degrees to 0 degrees
        myservo.write(pos); // tell servo to go to position in variable 'pos'
        delay(10);
        // waits 15ms for the servo to reach the position
    }
    for (pos = 120; pos <= 180; pos += 1)
    { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        myservo.write(pos); // tell servo to go to position in variable 'pos'
        delay(10);          // waits 15ms for the servo to reach the position
    }
    Serial.println("khul ja simsim");
}

uint8_t getFingerprintEnroll(uint8_t id, String name, String password, String mailiD)
{
    int p = -1;
    Serial.print("Waiting for a valid finger to enroll as #");
    Serial.println(id);

    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Image taken");

            break;
        case FINGERPRINT_NOFINGER:
            Serial.println(".");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("place finger");
            break;

        default:
            Serial.println("Unknown error");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("error");
            break;
        }
    }

    p = finger.image2Tz(1);

    Serial.println("Remove finger");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("remove finger");
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER)
    {
        p = finger.getImage();
    }
    Serial.print("ID ");
    Serial.println(id);
    p = -1;
    Serial.println("Place the same finger again");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("place again");
    delay(2000);

    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.print(".");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("place finger");
            break;
            // case FINGERPRINT_PACKETRECIEVEERR:

        default:
            Serial.println("Unknown error");
            break;
        }
    }

    p = finger.image2Tz(2);

    p = finger.createModel();
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Prints matched!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("prints matched");
        delay(2000);
        member temp_member(name, id, false, password, mailiD);
        members.push_back(temp_member);
        saveToCSVFile(filename, members);
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("error");
        return p;
    }
    else if (p == FINGERPRINT_ENROLLMISMATCH)
    {
        Serial.println("Fingerprints did not match");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("did not match");
        delay(2000);
        return p;
    }
    else
    {
        Serial.println("Unknown error");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("error");
        return p;
    }

    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Stored!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("stored");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("error");
        return p;
    }

    else
    {
        Serial.println("Unknown error");
        return p;
    }
    return true;
}

void getFingerprintID()
{
    uint8_t p = finger.getImage();
    p = finger.image2Tz();
    p = finger.fingerSearch();

    if (p == FINGERPRINT_OK)
    {

        member &foundMember = getMemberByID(finger.fingerID);

        Serial.print("Name: ");

        Serial.print(foundMember.name);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(foundMember.name);
        lcd.print("  ");

        Serial.print("  ID: ");
        Serial.print(finger.fingerID);
        lcd.print(finger.fingerID);
        Serial.print(" Confidence: ");
        Serial.println(finger.confidence);
        if (foundMember.inside_status)
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("bye ");
            lcd.print(foundMember.name);
            foundMember.inside_status = !foundMember.inside_status;
            delay(2000);
            return;
        }
        else
        {
            // check if cordi
            bool is_cordi = false;
            for (auto i : cordi_ids)
                if (foundMember.id == i)
                    is_cordi = true;

            // if not cordi send otp mail
            if (!is_cordi)
            {
                String generated_OTP = generateOTP();
                for (auto i : cordi_ids)
                {
                    member &cordi = getMemberByID(i);
                    String content = foundMember.name + " is trying to acces the room.\nOTP = " + generated_OTP;
                    sendMail(cordi.mailiD, "OTP to access club", content);
                }
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("enter otp");
                String entered_otp = "";

                // keypad input
                while (len(entered_otp) != 4)
                {
                    // char customKey = customKeypad.getKey();
                    char customKey = Read_TTP229_Keypad();
                    if (customKey)
                    {
                        entered_otp += customKey;
                        lcd.clear();
                        lcd.setCursor(0, 0);
                        lcd.print(entered_otp);
                    }
                }

                // if not match
                if (entered_otp != generated_OTP)
                {
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Wrong otp");
                    delay(3000);
                    return;
                }
            }

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("hello ");
            lcd.print(foundMember.name);
            foundMember.inside_status = !foundMember.inside_status;
            String content = foundMember.name + " has accessed the club through biometric authentication";
            sendMail((String)AUTHOR_EMAIL, "Club Opened", content);
            unlock();
            delay(2000);
            return;
        }

        // return finger.fingerID;
        return;
    }

    else
    {
        Serial.println("Did not find a match");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("not found");
        delay(2000);
        // return p;
        return;
    }
}

int memberLocation(uint8_t iD)
{
    member &foundMember = getMemberByID(iD);

    int i = 0;

    while (members[i].id != foundMember.id)
    {
        i++;
    }
    return i;
}

void loop()
{

    server.handleClient();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("eclub");
    if (finger.getImage() == !FINGERPRINT_NOFINGER)
    {
        getFingerprintID();
    }

    // finger.emptyDatabase();
    // members.clear();

    // saveToCSVFile(filename, members);
}

void saveToCSVFile(const char *path, const vector<member> &data)
{
    File file = SD.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    for (const auto &entry : members)
    {
        file.print(entry.name);
        file.print(",");
        file.print(entry.id);
        file.print(",");
        file.print(entry.password); // Add password to CSV file
        file.print(",");
        file.print(entry.mailiD); // Add this line for email ID
        file.print(",");
        file.println(entry.inside_status);
    }

    Serial.println("CSV file saved successfully");

    file.close();
}

void loadDataFromCSVFile(const char *path, std::vector<member> &members)
{
    File file = SD.open(path);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    while (file.available())
    {
        String line = file.readStringUntil('\n');
        if (line.length() > 0)
        {
            int firstCommaIndex = line.indexOf(',');
            int secondCommaIndex = line.indexOf(',', firstCommaIndex + 1);
            int thirdCommaIndex = line.indexOf(',', secondCommaIndex + 1);
            int fourthCommaIndex = line.indexOf(',', thirdCommaIndex + 1);

            if (firstCommaIndex != -1 && secondCommaIndex != -1 && thirdCommaIndex != -1)
            {
                String key = line.substring(0, firstCommaIndex);
                String valueStr = line.substring(firstCommaIndex + 1, secondCommaIndex);
                int value = valueStr.toInt();
                String password = line.substring(secondCommaIndex + 1, thirdCommaIndex);
                String statusStr = line.substring(thirdCommaIndex + 1, fourthCommaIndex);
                bool status = statusStr.toInt();
                String mailiD = line.substring(fourthCommaIndex + 1); // Add this line for email ID

                members.push_back(member(key, value, status, password, mailiD));
            }
        }
    }

    Serial.println("Data loaded from CSV file");
    file.close();
}

void handleRoot()
{

    String html = "<html><head><meta http-equiv='refresh' content='1'></head><body>"; // Refresh every 10 seconds
    html += "<h1>Member Data</h1>";
    html += "<table border='1'><tr><th>Name</th><th>ID</th><th>Inside Status</th></tr>";

    for (const member &m : members)
    {
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

void handleFormPage()
{
    String mailID = server.arg("mailID");

    String adminOTP = generateOTP();
    Serial.println("admin otp " + adminOTP);
    String content = "OTP to authorise " + mailID + "to enter the World of Electronics is " + adminOTP + ".";

    sendMail((String)AUTHOR_EMAIL, "One Time Password", content);

    String formHtml = "<html><body><h2>Registration Form</h2>";
    formHtml += "<form action='/register' method='post'>";
    formHtml += "Name: <input type='text' name='name'><br>";
    formHtml += "ID: <input type='number' name='id'><br>";
    formHtml += "Create Password: <input type='password' name='password'><br>"; // Member password
    formHtml += "OTP: <input type='password' name='otp'><br>";                  // Admin password
    formHtml += "<input type='hidden' name='mailID' value='" + mailID + "'>";
    formHtml += "<input type='hidden' name='adminOTP' value='" + adminOTP + "'>";
    formHtml += "<input type='submit' value='Register'>";
    formHtml += "</form></body></html>";
    server.send(200, "text/html", formHtml);
}

void handleMemberSubmit()
{
    String memberName = server.arg("name");
    int memberID = server.arg("id").toInt();
    String memberPassword = server.arg("password");
    String otp = server.arg("otp");
    String mailID = server.arg("mailID");
    String adminOTP = server.arg("adminOTP");

    if (adminOTP == otp)
    {
        getFingerprintEnroll(memberID, memberName, memberPassword, mailID);
        String content = "Dear " + memberName + ",you have been successfully authorised to access the World of Electronics.";
        sendMail(mailID, "Registration Complete", content);
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "");
    }

    else
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("invalid OTP");
        String content = "Dear " + memberName + ", the OTP entered was incorrect or the iD is already taken. Please try again.";
        sendMail(mailID, "Registration Failed", content);
        delay(2000);
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "Invalid admin password.");
    }
}

void handleUnlockPage()
{

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
void handleUnlock()
{
    String id = server.arg("id");
    String enteredPassword = server.arg("password");
    String mailID = server.arg("mailID");
    int memberId = id.toInt();

    for (const member &m : members)
    {
        if (m.id == memberId && m.password == enteredPassword && m.mailiD == mailID)
        {
            member &foundMember = getMemberByID(m.id);
            foundMember.inside_status = !foundMember.inside_status;
            String content = foundMember.name + " has accessed the Electronics Club through the web page.";
            sendMail((String)AUTHOR_EMAIL, "Club Opened", content);
            unlock();
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("door unlocked");
            delay(3000);
            server.sendHeader("Location", "/", true);
            server.send(302, "text/plain", "");
            return;
        }
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("invalid creds");
    delay(3000);
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

void handleChangepasspage()
{
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
void handleChangepass()
{
    uint8_t id = server.arg("id").toInt();
    String newpass = server.arg("newpass");
    String name = server.arg("name");
    String mailID = server.arg("mailID");

    member &foundMember = getMemberByID(id);

    if (foundMember.name == name && foundMember.mailiD == mailID)
    {
        foundMember.password = newpass;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("pass changed");
        delay(2000);
    }
    else
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("invalid creds");
        delay(2000);
    }
    delay(3000);
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

void handleDeleteuserPage()
{
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
void handleDeleteuser()
{
    uint8_t iD = server.arg("id").toInt();
    String name = server.arg("name");
    String adminOTP = server.arg("adminOTP");
    String otp = server.arg("otp");
    member &foundMember = getMemberByID(iD);
    if (foundMember.name == name && foundMember.name == name && otp == adminOTP)
    {
        String content = "Dear " + foundMember.name + ",you have been successfully deratified and barred from entering the Electroncis Club.";
        sendMail(foundMember.mailiD, "Deratified", content);
        members.erase(members.begin() + memberLocation(iD));
        finger.deleteModel(iD);
        saveToCSVFile(filename, members);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("deratified");
        delay(2000);
    }

    else
    {
        String content = "Dear " + foundMember.name + ", you were just on the brink of getting deratified from the Electronics Club.";
        sendMail(foundMember.mailiD, "Deratification Failed", content);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("invalid creds");
        delay(2000);
    }
    delay(3000);
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

void handleOTPVerification()
{
    String action = server.arg("action");
    String otpHtml = "<html><body><h2>OTP Verification</h2>";
    otpHtml += "<form action='/otpverification' method='post'>";
    otpHtml += "Enter your Mail ID: <input type='text' name='mailID'><br>";
    otpHtml += "<input type='submit' value='Send OTP'>";
    otpHtml += "<input type='hidden' name='action' value='" + action + "'>";
    otpHtml += "</form></body></html>";
    server.send(200, "text/html", otpHtml);
}
void handleOTPVerificationPage()
{
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

void handleVerifyOTP()
{
    String enteredOTP = server.arg("otp");
    String action = server.arg("action");
    String mailID = server.arg("mailID");
    if (enteredOTP == generatedOTP)
    {
        server.sendHeader("Location", "/" + action + "?mailID=" + mailID, true);
        server.send(302, "text/plain", "");
    }
    else
    {

        String content = "The OTP you have provided is incorrect, Please try again.";
        sendMail(mailID, "Incorrect OTP", content);

        server.sendHeader("Location", "/?error=Invalid OTP", true);
        server.send(302, "text/plain", "");
    }
}
