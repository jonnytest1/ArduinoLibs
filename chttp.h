
#include <Arduino.h>
#include <map>

using namespace std::__cxx11;

class CHTTP
{

public:
    CHTTP() = delete;
    CHTTP(char *new_ssid, char *new_password);
    void request(const String url, const std::map<String, String> data, void (*callback)(int httpCode, String response), boolean b64);
};
