#include "chttp.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

using namespace std::__cxx11;

char *wifissid = "";
char *wifipassword = "";

CHTTP::CHTTP(char *new_ssid, char *new_password)
{
    wifissid = new_ssid;
    wifipassword = new_password;
}

String json(const std::map<String, String> content)
{
    String out = "{";
    std::map<String, String>::const_iterator it = content.begin();
    bool separator = false;
    while (it != content.end())
    {
        if (separator)
        {
            out = out + ",";
        }
        else
        {
            separator = true;
        }
        out = out + "\"" + it->first + "\":\"" + it->second + "\" \n ";
        it++;
    }
    return out + "}";
}

String base64_encode(const String &in)
{

    string out;

    int val = 0, valb = -6;
    for (char c : in)
    {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0)
        {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
        out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4)
        out.push_back('=');
    return out.c_str();
}

void CHTTP::request(const String url, const std::map<String, String> data, void (*callback)(int httpCode, String response), boolean b64)
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.begin(wifissid, wifipassword);
    int t = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        t++;
        delay(100);
        Serial.println("connecting ... " + WiFi.status());
        if (t > 50)
        {
            Serial.print("Connecting failed");
            return;
        }
    }
    HTTPClient httpf;
    httpf.begin(url);

    String requestData = json(data);
    if (b64)
    {
        requestData = base64_encode(requestData);
    }
    else
    {
        httpf.addHeader("content-type", "application/json");
    }
    Serial.println(requestData);
    int httpCodef = httpf.POST(requestData);
    if (httpCodef > 0)
    {
        Serial.printf("[HTTP] POST... code: %d\n", httpCodef);
        if (callback != NULL)
        {
            callback(httpCodef, httpf.getString());
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", httpf.errorToString(httpCodef).c_str());
    }
    httpf.end();
}