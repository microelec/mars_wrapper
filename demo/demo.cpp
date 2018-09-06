#include "mars_plus.h"
using namespace marsPlus;

//推送监听类
class PushHandler :PushListener {
    virtual void onPush(const std::string &message) {

    }
};
//应答监听类
class ResponseHandler :ResponseListener {
    virtual void onResponse(const std::string &message) {
        printf("response received:%s \n",message.c_str());
    }
    virtual void onError(const int err, const std::string &errMsg) {
        printf("message send failed:%d \n",err);
    }
    virtual void onSuccess() {
        printf("message send ok \n");
    }
};

int main(int argc, char* argv[]) {
    MarsConfig config("39.106.56.27",9001);//connect to a test server
    init(config);
    PushHandler pushHandler;
    registerPushListener((PushListener*)&pushHandler);
    _sleep(2000);
    ResponseHandler responseHandler;
    std::string message = "hello";
    sendMessage(message.c_str(), message.size(), (ResponseListener*)&responseHandler);
    _sleep(200000);
    return 0;
}