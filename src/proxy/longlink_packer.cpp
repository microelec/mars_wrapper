#include "longlink_packer.h"
//#include <arpa/inet.h>
#ifdef WIN32

#include<Winsock2.h>
#else
#include <arpa/inet.h>
#endif // DEBUG
#include "mars/xlog/xlogger.h"
#include "mars/comm/autobuffer.h"
#include "mars/stn/stn.h"
#include "mars/stn/stn_logic.h"
#include "mars_task_proxy.h"
#include "im_message.h"

static uint32_t sg_client_version = 0;
//static uint16_t echat_protocal_version = 1;
static uint32_t im_heartbeat_bytes=4;
#define SIGNALKEEP_CMDID 243
#define PUSH_DATA_TASKID_START 0
#define PUSH_DATA_TASKID_END 10000
//自定义长连加解包
//https://github.com/Tencent/mars/wiki/Mars-自定义扩展
namespace mars {
    namespace stn {
        longlink_tracker* (*longlink_tracker::Create)()
        = []() {
            return new longlink_tracker;
        };
        
        void SetClientVersion(uint32_t _client_version)  {
            sg_client_version = _client_version;
        }
        
        
        //NOLINTNEXTLINE
        static int __unpack_test(const void* _packed, size_t _packed_len, uint32_t& _cmdid, uint32_t& _seq, size_t& _package_len, size_t& _body_len) {
            const ImMessage::MessageFormat *header = reinterpret_cast<const ImMessage::MessageFormat*>(_packed);
            if (_packed_len == im_heartbeat_bytes) { //heart beat
                _seq = Task::kNoopTaskID;
                _package_len = _packed_len;
                return LONGLINK_UNPACK_OK;
            }
            _cmdid = 0;
            _seq = header->messageId;
            _body_len = header->len;
            _package_len = header->len+sizeof(ImMessage::MessageFormat);
            xdebug2(TSF"__unpack_test:cmdid %_,taskid %_,package_size %_",_cmdid,_seq,_body_len);
            
            if (_package_len > 1024*1024) { return LONGLINK_UNPACK_FALSE; }
            if (_package_len > _packed_len) { return LONGLINK_UNPACK_CONTINUE; }

            return LONGLINK_UNPACK_OK;
        }
        
        //void longlink_pack(uint32_t _cmdid, uint32_t _seq, uint8_t _qosLevel, const void*  _raw, size_t _raw_len, AutoBuffer& _packed) {
        //NOLINTNEXTLINE
        void (*longlink_pack)(uint32_t _cmdid, uint32_t _seq, const AutoBuffer& _body, const AutoBuffer& _extension, AutoBuffer& _packed, longlink_tracker* _tracker)//NOLINTNEXTLINE
        = [](uint32_t _cmdid, uint32_t _seq, const AutoBuffer& _body, const AutoBuffer& _extension, AutoBuffer& _packed, longlink_tracker* _tracker) {
            
            if(_seq == Task::kNoopTaskID){ // heartbeat
                uint32_t heartBeatPackage = htonl(0x00000004); // defined
                _packed.AllocWrite(sizeof(heartBeatPackage));
                _packed.Write(&heartBeatPackage,sizeof(heartBeatPackage));
            }
            else
            {
                _packed.AllocWrite(_body.Length());
                if (_body.Length()>0)
                {
                    xdebug2(TSF"longlink_pack:cmdid %_,taskid %_,package_size %_",_cmdid,_seq,_body.Length());
                    _packed.Write(_body.Ptr(), _body.Length());
                }
            }
            _packed.Seek(0, AutoBuffer::ESeekStart);
        };
        
        
        //int longlink_unpack(const AutoBuffer& _packed, uint32_t& _cmdid, uint32_t& _seq, size_t& _package_len, AutoBuffer& _body) {
        //NOLINTNEXTLINE
        int (*longlink_unpack)(const AutoBuffer& _packed, uint32_t& _cmdid, uint32_t& _seq, size_t& _package_len, AutoBuffer& _body, AutoBuffer& _extension, longlink_tracker* _tracker)//NOLINTNEXTLINE
        = [](const AutoBuffer& _packed, uint32_t& _cmdid, uint32_t& _seq, size_t& _package_len, AutoBuffer& _body, AutoBuffer& _extension, longlink_tracker* _tracker) {
            
            size_t body_len = 0;
            int ret = __unpack_test(_packed.Ptr(), _packed.Length(), _cmdid,  _seq, _package_len, body_len);
            
            if (LONGLINK_UNPACK_OK != ret) return ret;
            
            _body.Write(AutoBuffer::ESeekCur, _packed.Ptr(0), _packed.Length());
            //_body.Write(AutoBuffer::ESeekCur, _packed.Ptr(_package_len-body_len), body_len);
            
            return ret;
        };
        
        uint32_t (*longlink_noop_cmdid)()
        = []() -> uint32_t {
            return Task::kNoopTaskID;
        };
        
        bool  (*longlink_noop_isresp)(uint32_t _taskid, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend)
        = [](uint32_t _taskid, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend) {
            return Task::kNoopTaskID == _recv_seq;
        };
        
        uint32_t (*signal_keep_cmdid)()
        = []() -> uint32_t {
            return SIGNALKEEP_CMDID;
        };
        //NOLINTNEXTLINE
        void (*longlink_noop_req_body)(AutoBuffer& _body, AutoBuffer& _extend)//NOLINTNEXTLINE
        = [](AutoBuffer& _body, AutoBuffer& _extend) {
            
            //SendEChatHeartBeat();
            char hb[] = "dummy";
            _body.Write(hb, sizeof(hb));
        };
        
        void (*longlink_noop_resp_body)(const AutoBuffer& _body, const AutoBuffer& _extend)
        = [](const AutoBuffer& _body, const AutoBuffer& _extend) {
            
        };
        
        uint32_t (*longlink_noop_interval)()
        = []() -> uint32_t {
            return 10000;
        };
        
        bool (*longlink_complexconnect_need_verify)()
        = []() {
            return false;
        };
        
        bool (*longlink_ispush)(uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend)
        = [](uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend) {
            return _taskid>PUSH_DATA_TASKID_START && _taskid<PUSH_DATA_TASKID_END;
        };
        
        bool (*longlink_identify_isresp)(uint32_t _sent_seq, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend)
        = [](uint32_t _sent_seq, uint32_t _cmdid, uint32_t _recv_seq, const AutoBuffer& _body, const AutoBuffer& _extend) {
            return _sent_seq == _recv_seq && 0 != _sent_seq;
        };
        
    }//namespace stn
}//namespace mars
