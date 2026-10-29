#pragma once
#include "../core/SearchEngine.h"
#include <string>

class HttpServer {
public:
    explicit HttpServer(SearchEngine& engine, int port=8080);
    void run();

private:
    SearchEngine& engine_;
    int port_;

    std::string handle(const std::string& method,const std::string& target,const std::string& body);
    static std::string urlDecode(const std::string& s);
    static std::string jsonEscape(const std::string& s);
    static std::string getQuery(const std::string& target,const std::string& key);
    static std::string response(const std::string& body,int status=200);
};
