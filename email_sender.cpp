#include "email_sender.h"

SMTPSession smtp;

void smtpCallback(SMTP_Status status) {
  // Print only the final status of the sent message
  if (status.success()) {
    Serial.println("Message sent successfully");
  } else {
    Serial.println("Message send failed");
  }
}

void setupEmailSender() {
    smtp.debug(0);
    smtp.callback(smtpCallback);
}

void sendMail(String recipientEmail, String subject, String content) {
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
  if (!smtp.connect(&config)) {
    return;
  }

  // Start sending Email and close the session
  MailClient.sendMail(&smtp, &message);
}