
#ifndef stn_callback_hpp
#define stn_callback_hpp

#include <stdio.h>

#include "mars/stn/stn_logic.h"
#include "mars/stn/stn.h"
#include "mars_plus.h"
#include "im_message.h"
namespace marsPlus {
    
class StnCallBack : public mars::stn::Callback {
    
private:
    StnCallBack() {}
    ~StnCallBack() {}
    StnCallBack(StnCallBack&);
    StnCallBack& operator = (StnCallBack&);
    
public:
    static StnCallBack* Instance();
    static void Release();
    
    virtual bool MakesureAuthed();
    
    //流量统计
    virtual void TrafficData(ssize_t _send, ssize_t _recv);
    
    //底层询问上层该host对应的ip列表
    virtual std::vector<std::string> OnNewDns(const std::string& _host);
    //网络层收到push消息回调
    //NOLINTNEXTLINE
    virtual void OnPush(uint64_t _channel_id, uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend);
    //底层获取task要发送的数据
    //NOLINTNEXTLINE
    virtual bool Req2Buf(uint32_t _taskid, void* const _user_context, AutoBuffer& outbuffer, AutoBuffer& extend, int& error_code, const int channel_select);
    //底层回包返回给上层解析
    //NOLINTNEXTLINE
    virtual int Buf2Resp(uint32_t _taskid, void* const _user_context, const AutoBuffer& _inbuffer, const AutoBuffer& _extend, int& _error_code, const int _channel_select);
    //任务执行结束
    virtual int  OnTaskEnd(uint32_t _taskid, void* const _user_context, int _error_type, int _error_code);
    
    //上报网络连接状态
    virtual void ReportConnectStatus(int _status, int _longlink_status);
    //长连信令校验 ECHECK_NOW, ECHECK_NEVER = 1, ECHECK_NEXT = 2
    //NOLINTNEXTLINE
    virtual int  GetLonglinkIdentifyCheckBuffer(AutoBuffer& _identify_buffer, AutoBuffer& _buffer_hash, int32_t& _cmdid);
    //长连信令校验回包
    //NOLINTNEXTLINE
    virtual bool OnLonglinkIdentifyResponse(const AutoBuffer& _response_buffer, const AutoBuffer& _identify_buffer_hash);
    //
    virtual void RequestSync();
    //验证是否已登录
    virtual bool IsLogoned();
    
private:
    static StnCallBack* _instance;
    
    static void buildEchatTcpMsg(ImMessage *msg, AutoBuffer& outbuffer); //NOLINT
};

PushListener * getPushListener();
ChannelListener * getChannelListener();

} //namespace jupiter

#endif /* stn_callback_hpp */
