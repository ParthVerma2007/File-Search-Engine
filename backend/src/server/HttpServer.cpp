#include "HttpServer.h"
#include <algorithm>
#include <cstring>
#include <sstream>
#include <thread>
#include <iostream>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using Socket=SOCKET;
static void closeSocket(Socket s){closesocket(s);}
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using Socket=int;
static void closeSocket(Socket s){close(s);}
#endif

HttpServer::HttpServer(SearchEngine& engine,int port):engine_(engine),port_(port){}

std::string HttpServer::urlDecode(const std::string& s){
    std::string out;
    for(std::size_t i=0;i<s.size();++i){
        if(s[i]=='%'&&i+2<s.size()){
            int v=0; std::stringstream ss; ss<<std::hex<<s.substr(i+1,2); ss>>v;
            out.push_back((char)v); i+=2;
        }else if(s[i]=='+')out.push_back(' ');
        else out.push_back(s[i]);
    }
    return out;
}
std::string HttpServer::jsonEscape(const std::string& s){
    std::string o;
    for(char c:s){
        if(c=='"')o+="\\\"";
        else if(c=='\\')o+="\\\\";
        else if(c=='\n')o+="\\n";
        else if(c=='\r')o+="\\r";
        else o+=c;
    }
    return o;
}
std::string HttpServer::getQuery(const std::string& target,const std::string& key){
    auto q=target.find('?'); if(q==std::string::npos)return "";
    std::string query=target.substr(q+1);
    std::stringstream ss(query); std::string item;
    while(std::getline(ss,item,'&')){
        auto p=item.find('=');
        if(p!=std::string::npos&&urlDecode(item.substr(0,p))==key)return urlDecode(item.substr(p+1));
    }
    return "";
}
std::string HttpServer::response(const std::string& body,int status){
    std::string statusText=status==200?"OK":status==404?"Not Found":"Bad Request";
    return "HTTP/1.1 "+std::to_string(status)+" "+statusText+"\r\n"
           "Content-Type: application/json; charset=utf-8\r\n"
           "Access-Control-Allow-Origin: *\r\n"
           "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
           "Access-Control-Allow-Headers: Content-Type\r\n"
           "Content-Length: "+std::to_string(body.size())+"\r\n"
           "Connection: close\r\n\r\n"+body;
}
std::string HttpServer::handle(const std::string& method,const std::string& target,const std::string&){
    auto path=target.substr(0,target.find('?'));
    if(method=="OPTIONS")return response("{}");
    if(path=="/api/search"){
        std::string q=getQuery(target,"q");
        std::string sa=getQuery(target,"search");
        std::string so=getQuery(target,"sort");
        if(sa.empty())sa="Binary Search";
        if(so.empty())so="Quick Sort";
        SearchStats st;
        auto results=engine_.search(q,sa,so,st);
        std::ostringstream j;
        j<<"{\"query\":\""<<jsonEscape(q)<<"\",\"searchAlgorithm\":\""<<jsonEscape(sa)
         <<"\",\"sortingAlgorithm\":\""<<jsonEscape(so)<<"\",\"stats\":{\"documents\":"
         <<st.documents<<",\"uniqueTerms\":"<<st.uniqueTerms<<",\"indexedTokens\":"
         <<st.indexedTokens<<",\"comparisons\":"<<st.comparisons<<",\"milliseconds\":"
         <<st.milliseconds<<"},\"results\":[";
        for(std::size_t i=0;i<results.size();++i){
            if(i)j<<',';
            const auto&r=results[i];
            j<<"{\"filename\":\""<<jsonEscape(r.filename)<<"\",\"score\":"<<r.score
             <<",\"snippet\":\""<<jsonEscape(r.snippet)<<"\",\"matchingTerms\":[";
            for(std::size_t k=0;k<r.matchingTerms.size();++k){
                if(k)j<<',';
                j<<"\""<<jsonEscape(r.matchingTerms[k])<<"\"";
            }
            j<<"]}";
        }
        j<<"]}";
        return response(j.str());
    }
    if(path=="/api/autocomplete"){
        auto p=getQuery(target,"prefix");
        auto words=engine_.autocomplete(p);
        std::ostringstream j;j<<"{\"prefix\":\""<<jsonEscape(p)<<"\",\"suggestions\":[";
        for(std::size_t i=0;i<words.size();++i){if(i)j<<',';j<<"\""<<jsonEscape(words[i])<<"\"";}
        j<<"]}";
        return response(j.str());
    }
    if(path=="/api/index/rebuild"&&method=="POST"){
        engine_.rebuild();
        return response("{\"message\":\"Index rebuilt successfully\"}");
    }
    if(path=="/api/stats"){
        const auto&s=engine_.indexStats();
        std::ostringstream j;
        j<<"{\"documents\":"<<s.documents<<",\"uniqueTerms\":"<<s.uniqueTerms
         <<",\"indexedTokens\":"<<s.indexedTokens<<"}";
        return response(j.str());
    }
    if(path=="/api/index/sample"){
        auto words=engine_.index().sample(30);
        std::ostringstream j;j<<"{\"terms\":[";
        for(std::size_t i=0;i<words.size();++i){if(i)j<<',';j<<"\""<<jsonEscape(words[i])<<"\"";}
        j<<"]}";
        return response(j.str());
    }
    return response("{\"error\":\"Endpoint not found\"}",404);
}

void HttpServer::run(){
#ifdef _WIN32
    WSADATA wsa{}; WSAStartup(MAKEWORD(2,2),&wsa);
#endif
    Socket server=socket(AF_INET,SOCK_STREAM,0);
    if(server<0)throw std::runtime_error("Could not create socket");

    int opt=1;
#ifdef _WIN32
    setsockopt(server,SOL_SOCKET,SO_REUSEADDR,(const char*)&opt,sizeof(opt));
#else
    setsockopt(server,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
#endif
    sockaddr_in addr{};addr.sin_family=AF_INET;addr.sin_addr.s_addr=INADDR_ANY;addr.sin_port=htons(port_);
    if(bind(server,(sockaddr*)&addr,sizeof(addr))<0)throw std::runtime_error("Could not bind port");
    if(listen(server,16)<0)throw std::runtime_error("Could not listen");

    std::cout<<"HTTP server running at http://127.0.0.1:"<<port_<<"\n";
    std::cout<<"Press Ctrl+C to stop.\n";

    while(true){
        Socket client=accept(server,nullptr,nullptr);
#ifdef _WIN32
        if(client==INVALID_SOCKET)continue;
#else
        if(client<0)continue;
#endif
        std::thread([this,client](){
            std::string req; char buf[8192];
            int n=0;
            do{
#ifdef _WIN32
                n=recv(client,buf,sizeof(buf),0);
#else
                n=(int)recv(client,buf,sizeof(buf),0);
#endif
                if(n>0)req.append(buf,n);
            }while(n==sizeof(buf)&&req.find("\r\n\r\n")==std::string::npos);

            auto head=req.find("\r\n\r\n");
            if(head==std::string::npos){closeSocket(client);return;}
            std::istringstream hs(req.substr(0,head));
            std::string method,target,version;hs>>method>>target>>version;
            std::string out=handle(method,target,head+4<req.size()?req.substr(head+4):"");
            send(client,out.c_str(),(int)out.size(),0);
            closeSocket(client);
        }).detach();
    }
}
