
#ifndef app_callback_hpp
#define app_callback_hpp

#include <stdio.h>
#include <string>
#include "mars/app/app_logic.h"

namespace marsPlus {
    
class AppCallBack : public mars::app::Callback {
    
private:
    AppCallBack() {}
    ~AppCallBack() {}
    AppCallBack(AppCallBack&);
    AppCallBack& operator = (AppCallBack&);
    
    
public:
    static AppCallBack* Instance();
    static void Release();
    
    virtual std::string GetAppFilePath();
    
    virtual mars::app::AccountInfo GetAccountInfo();
    
    virtual unsigned int GetClientVersion();
    
    virtual mars::app::DeviceInfo GetDeviceInfo();
    
private:
    static AppCallBack* _instance;
};
    
} 
#endif /* app_callback_hpp */
