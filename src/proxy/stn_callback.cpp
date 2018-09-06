
#include "stn_callback.h"
#include <stdio.h>
#include <mars/xlog/xlogger.h>
#include <string>
#include <vector>
#include "mars/stn/stn.h"
#include "mars/stn/stn_logic.h"
#include "mars_task_proxy.h"
#include "im_message.h"
marsPlus::StnCallBack* marsPlus::StnCallBack::_instance = NULL;

marsPlus::StnCallBack* marsPlus::StnCallBack::Instance() {
    if(_instance == NULL) {
        _instance = new StnCallBack();
    }
    
    return _instance;
}

void marsPlus::StnCallBack::Release() {
    delete _instance;
    _instance = NULL;
}

bool marsPlus::StnCallBack::MakesureAuthed() {
    return true;
}

void marsPlus::StnCallBack::TrafficData(ssize_t _send, ssize_t _recv) {
    
}

std::vector<std::string> marsPlus::StnCallBack::OnNewDns(const std::string& _host) {
    std::vector<std::string> vector;
    return vector;
}


/*
 收到server推送消息
*/
void marsPlus::StnCallBack::OnPush(uint64_t _channel_id, uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend) {
    xdebug2(TSF"onPush _taskid:%_,_cmdid:%_",_taskid,_cmdid);
    ImMessage pushMessage(reinterpret_cast<const char *>(_body.Ptr(0)), static_cast<int>(_body.Length()));
    //echatMessage.netToHost();
    //MarsTaskProxy::onPush(pushMessage);
    PushListener* pushListener = marsPlus::getPushListener();
    if (pushListener != NULL&&pushMessage.isValid()) {
        pushListener->onPush(pushMessage.getData());
    }
}

/*
 发送业务组包
 */
bool marsPlus::StnCallBack::Req2Buf(uint32_t _taskid, void* const _user_context, AutoBuffer& outbuffer, AutoBuffer& extend, int& error_code, const int channel_select) {
    
    const TaskContext* ctx = MarsTaskProxy::getTaskConext(_taskid);
    //xdebug2(TSF"req2Buf called: taskid %_, ctx %_, msg %_",_taskid,(uint64_t)ctx,(uint64_t)ctx->_msg);
    if( ctx && ctx->msg){
        ImMessage* msg = static_cast<ImMessage*>(ctx->msg);
        buildEchatTcpMsg(msg, outbuffer);
        return true;
    }
    else{
        xwarn2(TSF"task %_ context does not exist",_taskid);
        return false;
    }    
}

/*
 server的业务应答包，与req2buf发送包通过taskid关联
 */

int marsPlus::StnCallBack::Buf2Resp(uint32_t _taskid, void* const _user_context, const AutoBuffer& _inbuffer, const AutoBuffer& _extend, int& _error_code, const int _channel_select) {
    xdebug2(TSF"Buf2Resp _taskid:%_ ,error_code:%_ ",_taskid,_error_code);
    const TaskContext* ctx = MarsTaskProxy::getTaskConext(_taskid);
    //xdebug2(TSF"req2Buf called: taskid %_, ctx %_, msg %_",_taskid,(uint64_t)ctx,(uint64_t)ctx->_msg);
    if (ctx && ctx->respCallback!=NULL) {
        ResponseListener* callback = static_cast<ResponseListener *>(ctx->respCallback);
        ImMessage message;
        message.parse(static_cast<const char *>(_inbuffer.Ptr(0)), static_cast<int>(_inbuffer.Length()));
        callback->onResponse(message.getData());
        return 0;
    }
    else {
        xwarn2(TSF"task %_ context does not exist", _taskid);
        return -1;
    }

}

/*
 发送任务的最终状态回调
 */
int  marsPlus::StnCallBack::OnTaskEnd(uint32_t _taskid, void* const _user_context, int _error_type, int _error_code) {
    xdebug2(TSF"OnTaskEnd _taskid:%_ ,error_type %_,error_code:%_ ",_taskid,_error_type,_error_code);
    const TaskContext* ctx = MarsTaskProxy::getTaskConext(_taskid);
    if (ctx == NULL) return 0;
    // longlink tcp
    if (NULL != ctx->respCallback) {
        ResponseListener* callback = static_cast<ResponseListener *>(ctx->respCallback);
        if (_error_type == mars::stn::ErrCmdType::kEctOK) {
            callback->onSuccess();
        }
        else {
            callback->onError(_error_code, "request failed.");
        }
    }
    MarsTaskProxy::removeTaskContext(_taskid);
    
    return 0;
}


void marsPlus::StnCallBack::ReportConnectStatus(int _status, int _longlink_status) {
    xinfo2(TSF"mars long link status %_",_longlink_status);
    ChannelListener* channelListener = marsPlus::getChannelListener();
    if (channelListener != NULL) channelListener->onChannelStatus(_longlink_status);
}

// synccheck：长链成功后由网络组件触发
// 需要组件组包，发送一个req过去，网络成功会有resp，但没有taskend，处理事务时要注意网络时序
// 不需组件组包，使用长链做一个sync，不用重试
int  marsPlus::StnCallBack::GetLonglinkIdentifyCheckBuffer(AutoBuffer& _identify_buffer, AutoBuffer& _buffer_hash, int32_t& _cmdid) {
    _cmdid = mars::stn::Task::kNoopTaskID;
    return 1;
}

bool marsPlus::StnCallBack::OnLonglinkIdentifyResponse(const AutoBuffer& _response_buffer, const AutoBuffer& _identify_buffer_hash) {
    
    return false;
}
//
void marsPlus::StnCallBack::RequestSync() {
    
}

bool marsPlus::StnCallBack::IsLogoned() {
    return true;
}

void marsPlus::StnCallBack::buildEchatTcpMsg(ImMessage *msg, AutoBuffer& outbuffer){    
    xinfo2(TSF"Req2Buf send msg -> header[messageId:%_,messageData:%_]", msg->getMessageId(), msg->getData());
    outbuffer.Write(msg->getMessage(),msg->getMessageSize());
}

extern marsPlus::PushListener *g_pushListener;
extern marsPlus::ChannelListener * g_channelListener;
marsPlus::PushListener * marsPlus::getPushListener() { return g_pushListener; }
marsPlus::ChannelListener * marsPlus::getChannelListener() { return g_channelListener; }


