#ifndef WEB_SERVER_H
#define WEB_SERVER_H


#include <WebServer.h>

String generateOTP();
void setupWebServer();
void serverHandle();
void handleRoot();
void handleFormPage();
void handleMemberSubmit();
void handleUnlockPage();
void handleUnlock();
void handleChangepasspage();
void handleChangepass();
void handleDeleteuserPage();
void handleDeleteuser();
void handleOTPVerification();
void handleOTPVerificationPage();
void handleVerifyOTP();


#endif