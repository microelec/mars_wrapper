#ifndef __MARS_PLUS_H__
#define __MARS_PLUS_H__
#include <stdint.h>
#include <string>

#ifdef WIN32
#define DllExport   __declspec( dllexport ) 
#else
#define DllExport 
#endif // WIN32

namespace marsPlus {
    // Mars configuration
    struct DllExport MarsConfig {
        std::string host;
        std::string backupIp;
        uint32_t port;
        MarsConfig(const std::string& host, const uint32_t port) {
            this->host = host;
            this->port = port;
        }
    };

    //longlink channel status listener interface
    class DllExport ChannelListener {
    public:
        /*longlink report status
        * idle = 0,
        * connecting = 1,
        * connected = 2,
        */
        virtual void onChannelStatus(int status) = 0;

    };

    //longlink channel push message listener interface
    class DllExport ResponseListener {
    public:
        virtual void onResponse(const std::string &message) = 0;
        virtual void onError(const int err,const std::string &errMsg) = 0;
        virtual void onSuccess() = 0;
    };

    //longlink channel push message listener interface
    class DllExport PushListener {
    public:
        virtual void onPush(const std::string &message) = 0;

    };
    
    int DllExport init(const MarsConfig &config);
    int DllExport sendMessage(const char* body,const uint32_t size,const ResponseListener *callback=NULL);
    void DllExport registerPushListener(PushListener *const listener);
    void DllExport registerChannelListener( ChannelListener *const listener);



}

#endif // !__MARS_PLUS_H__


