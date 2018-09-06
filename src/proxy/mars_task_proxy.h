//
//  mars_task_proxy.hpp
//  jupiter
//
//  Created by fumw on 2017/7/18.
//  Copyright © 2017年 wedoctor. All rights reserved.
//

#ifndef mars_task_proxy_h
#define mars_task_proxy_h
#include <map>
#include <vector>
#include "mars_plus.h"
#include "base.h"
typedef struct TaskContext {
    int needAck;
    void *msg;
    unsigned int size;
    void * respCallback;
    TaskContext() {
        msg = NULL;
        size = 0;
        respCallback = NULL;
    }
    ~TaskContext() {

    }

}TaskContext;
class MarsTaskProxy {
public:

    static void storeContext(const uint32_t taskId,TaskContext *const ctx);
    //static void init(const marsPlus::MarsConfig &config);
    //static void initLog(TLogLevel logLevel);
    //清空发送队列中的消息，同时重新配置mars stn，并重连
    static void reset();
    //重连并重新发送队列中的消息
    static void redoTasks();
    
    static TaskContext* getTaskConext(uint32_t msgId);
    
    static void removeTaskContext(uint32_t msgId);
    
    static void reportChannelStatus(int status);


private:
    
    static std::map<uint32_t,TaskContext*> taskId2TaskCtx; // for tcp longlink
    static Mutex taskId2TaskCtxMutex;
    
   // static std::map<uint32_t,TaskContext*> _httpTaskId2Ctx; // for http shortlink
    //static Mutex _httpMutex;
    //static uint32_t _httpTaskId;
    
    //static marsPlus::WDListener* _listener;

    //static EChatProcessor* _echatProcessor;
    
    //static bool _canLogin;
};

#endif /* mars_task_proxy_h */

