#include "sendemail.h"
#include <FS.h>
SendEmail::SendEmail(const String& host, const int port, const String& user, const String& passwd, const int timeout, const bool ssl) :
        host(host), port(port), user(user), passwd(passwd), timeout(timeout), ssl(ssl), client((ssl) ? new WiFiClientSecure() : new WiFiClient())
{

}

String SendEmail::readClient()
{
        String r = client->readStringUntil('\n');
        r.trim();
        while (client->available()) r += client->readString();
        return r;
}

bool SendEmail::send(const String& from, const String& to, const String& subject, const String& msg, const boolean isfile)
{
        if (!host.length())
        {
                return false;
        }
        client->stop();
        client->setTimeout(timeout);
        // smtp connect
#ifdef DEBUG_EMAIL_PORT
        Log +=("Connecting: ");
        Log +=(host);
        Log +=(":");
        Log +=(port);
#endif
        if (!client->connect(host.c_str(), port))
        {
#ifdef DEBUG_EMAIL_PORT
                Log +=("failed");
#endif
                return false;
        }
        String buffer = readClient();
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        if (!buffer.startsWith(F("220")))
        {
                return false;
        }
        buffer = F("EHLO ");
        IPAddress ipaddress = client->localIP();
        buffer += ipaddress[0];
        buffer += F(".");
        buffer += ipaddress[1];
        buffer += F(".");
        buffer += ipaddress[2];
        buffer += F(".");
        buffer += ipaddress[3];
        client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        buffer = readClient();
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        if (!buffer.startsWith(F("250")))
        {
                return false;
        }
        if (user.length()>0  && passwd.length()>0 )
        {
                buffer = F("AUTH LOGIN");
                client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
                Log +=(buffer);
#endif
                buffer = readClient();
#ifdef DEBUG_EMAIL_PORT
                Log +=(buffer);
#endif
                if (!buffer.startsWith(F("334")))
                {
                        return false;
                }
                base64 b;
                buffer = user;
                buffer = b.encode(buffer);
                client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
                Log +=(buffer);
#endif
                buffer = readClient();
#ifdef DEBUG_EMAIL_PORT
                Log +=(buffer);
#endif
                if (!buffer.startsWith(F("334")))
                {
                        return false;
                }
                buffer = this->passwd;
                buffer = b.encode(buffer);
                client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
                Log +=(buffer);
#endif
                buffer = readClient();
#ifdef DEBUG_EMAIL_PORT
                Log +=(buffer);
#endif
                if (!buffer.startsWith(F("235")))
                {
                        return false;
                }
        }
        // smtp send mail
        buffer = F("MAIL FROM: ");
        buffer += from;
        client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        buffer = readClient();
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        if (!buffer.startsWith(F("250")))
        {
                return false;
        }
        buffer = F("RCPT TO: ");
        buffer += to;
        client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        buffer = readClient();
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        if (!buffer.startsWith(F("250")))
        {
                return false;
        }
        buffer = F("DATA");
        client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        buffer = readClient();
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        if (!buffer.startsWith(F("354")))
        {
                return false;
        }
        buffer = F("From: ");
        buffer += from;
        client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        buffer = F("To: ");
        buffer += to;
        client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        buffer = F("Subject: ");
        buffer += subject;
        buffer += F("\r\n");
        client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        if (isfile)
        {
                File f;
                f = SPIFFS.open(msg, "r");
                if (f && f.size() < 3200000)
                {
                        while (f.available())
                                client->print((char)f.read());
                        f.close();
                }
                else
                        client->println("could not read file");
        }
        else
        {
                buffer = msg;
                client->println(buffer);
        }
        client->println('.');
        client->println('.');
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        buffer = F("QUIT");
        client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
        Log +=(buffer);
#endif
        return true;
}
