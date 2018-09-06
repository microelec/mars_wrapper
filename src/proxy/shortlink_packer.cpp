#include "shortlink_packer.h"
#include "mars/comm/http.h"

//using namespace http;
namespace mars { namespace stn {
    
    shortlink_tracker* (*shortlink_tracker::Create)()
    =  []() { return new shortlink_tracker; };
    //NOLINTNEXTLINE
    void (*shortlink_pack)(const std::string& _url, const std::map<std::string, std::string>& _headers, const AutoBuffer& _body, const AutoBuffer& _extension, AutoBuffer& _out_buff, shortlink_tracker* _tracker)//NOLINTNEXTLINE
    = [](const std::string& _url, const std::map<std::string, std::string>& _headers, const AutoBuffer& _body, const AutoBuffer& _extension, AutoBuffer& _out_buff, shortlink_tracker* _tracker) {
        bool isPost = (_url.find("?")==std::string::npos);
        http::Builder req_builder(http::kRequest);
        if(isPost){
            req_builder.Request().Method(http::RequestLine::kPost);
        }
        else{
            req_builder.Request().Method(http::RequestLine::kGet);
        }
        req_builder.Request().Version(http::kVersion_1_1);
        
        req_builder.Fields().HeaderFiled(http::HeaderFields::MakeAcceptAll());
        req_builder.Fields().HeaderFiled(http::HeaderFields::KStringUserAgent, http::HeaderFields::KStringMicroMessenger);
        req_builder.Fields().HeaderFiled(http::HeaderFields::MakeCacheControlNoCache());
        req_builder.Fields().HeaderFiled(http::HeaderFields::MakeConnectionClose());
        
        for (std::map<std::string, std::string>::const_iterator iter = _headers.begin(); iter != _headers.end(); ++iter) {
            req_builder.Fields().HeaderFiled(iter->first.c_str(), iter->second.c_str());
        }
        
        req_builder.Request().Url(_url);
        
        if(isPost){// POST
            req_builder.Fields().HeaderFiled("Content-Type","application/json");
            char len_str[32] = {0};
            snprintf(len_str, sizeof(len_str), "%u", (unsigned int)_body.Length());
            req_builder.Fields().HeaderFiled(http::HeaderFields::KStringContentLength, len_str);
            req_builder.HeaderToBuffer(_out_buff);
            _out_buff.Write(_body.Ptr(), _body.Length());
        }
        else{
            req_builder.HeaderToBuffer(_out_buff);
        }
        
    };
    
}// namespace stn
    
}// namespace mars

