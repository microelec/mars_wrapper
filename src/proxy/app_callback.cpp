
#include "app_callback.h"
#include <string>

marsPlus::AppCallBack* marsPlus::AppCallBack::_instance = NULL;

marsPlus::AppCallBack* marsPlus::AppCallBack::Instance() {
    if(_instance == NULL) {
        _instance = new AppCallBack();
    }
    
    return _instance;
}

void marsPlus::AppCallBack::Release() {
    delete _instance;
    _instance = NULL;
}

// return your app path
std::string marsPlus::AppCallBack::GetAppFilePath(){
    return "";
}

mars::app::AccountInfo marsPlus::AppCallBack::GetAccountInfo() {
    mars::app::AccountInfo info;
    
    return info;
}

unsigned int marsPlus::AppCallBack::GetClientVersion() {
    
    return 0;
}

mars::app::DeviceInfo marsPlus::AppCallBack::GetDeviceInfo() {
    mars::app::DeviceInfo info;
    
    info.devicename = "";
    info.devicetype = 1;
    
    return info;
}
