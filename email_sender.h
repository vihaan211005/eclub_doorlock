#ifndef EMAIL_SENDER_H
#define EMAIL_SENDER_H

#include <ESP_Mail_Client.h>

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "doorlock22eclub@gmail.com"
#define AUTHOR_PASSWORD "aorv fsxj uuoq bsfa"

extern SMTPSession smtp;

void smtpCallback(SMTP_Status status);
void setupEmailSender();
void sendMail(String recipientEmail, String subject, String content);

#endif