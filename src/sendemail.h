#ifndef __SENDEMAIL_H
#define __SENDEMAIL_H

//#define DEBUG_EMAIL_PORT Log

#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <base64.h>

//extern String Log;

class SendEmail
{
  private:
    const String host;
    const int port;
    const String user;
    const String passwd;
    const int timeout;
    const bool ssl;
    WiFiClient* client;
    String readClient();
  public:
   SendEmail(const String& host, const int port, const String& user, const String& passwd, const int timeout, const bool ssl);
   bool send(const String& from, const String& to, const String& subject, const String& msg, const boolean isfile);
   ~SendEmail() {client->stop(); delete client;}
};

#endif
