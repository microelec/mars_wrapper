
#include "mars_task_proxy.h"
#include <sstream>
#include "mars/stn/stn.h"
#include "mars/baseevent/base_logic.h"
#include "mars/stn/stn_logic.h"
#include "mars/app/app_logic.h"
#include "mars/xlog/xlogger.h"
#include "mars/xlog/appender.h"
#include "base.h"
#include "im_message.h"

std::map<uint32_t,TaskContext*> MarsTaskProxy::taskId2TaskCtx;
Mutex MarsTaskProxy::taskId2TaskCtxMutex;

void MarsTaskProxy::storeContext(const uint32_t taskId,TaskContext * const ctx)
{
    taskId2TaskCtxMutex.lock();
    MarsTaskProxy::taskId2TaskCtx[taskId] = ctx;
    TaskContext *temp = getTaskConext(taskId);
    if (temp != NULL) {
        xdebug2(TSF "add TaskCtx success,key is %_", taskId);
    }
    else {
        xerror2(TSF "add TaskCtx failed,key is %_", taskId);
    }
    taskId2TaskCtxMutex.unlock();
}

//clear tasks and reinitialize network
void MarsTaskProxy::reset(){
    xinfo2(TSF"mars reset");
    mars::stn::Reset();
}


//重连并重新发送队列中的消息
void MarsTaskProxy::redoTasks(){
    mars::stn::RedoTasks();
}


TaskContext *MarsTaskProxy::getTaskConext(uint32_t msgId){
    std::map<uint32_t,TaskContext*> &taskCtx = taskId2TaskCtx;
    if (taskCtx.find(msgId) == taskCtx.end() ) {
        return NULL;
    }
    return taskCtx[msgId];
}

void MarsTaskProxy::removeTaskContext(uint32_t msgId){
    xinfo2(TSF "removeTaskContext[%_]",msgId);
    TaskContext *ctx =getTaskConext(msgId);
    if(ctx == NULL) return;
    if (ctx->msg != NULL) delete static_cast<ImMessage*>(ctx->msg);
    taskId2TaskCtxMutex.lock();
    MarsTaskProxy::taskId2TaskCtx.erase(msgId);
    taskId2TaskCtxMutex.unlock();

    delete ctx;
}





