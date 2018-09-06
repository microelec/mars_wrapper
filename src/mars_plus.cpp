#include "mars_plus.h"
#include "mars/stn/stn.h"
#include "mars/baseevent/base_logic.h"
#include "stn_callback.h"
#include "app_callback.h"
#include "mars/stn/stn_logic.h"
#include "mars/xlog/xlogger.h"
#include "mars/xlog/appender.h"
#include "im_message.h"
#include "mars_task_proxy.h"
marsPlus::PushListener* g_pushListener = NULL;
marsPlus::ChannelListener *g_channelListener = NULL;
int marsPlus::init(const MarsConfig & config)
{
    appender_set_console_log(true);
    //appender_open_with_cache((TAppenderMode)0, config.logFilePath.c_str(), config.logFilePath.c_str(), config.nameprefix.c_str(),"");
    appender_open((TAppenderMode)0, "./log", "marPlus", "");
    xlogger_SetLevel((TLogLevel)kLevelVerbose);


    std::vector<uint16_t> ports;
    ports.push_back(config.port);
    mars::stn::SetLonglinkSvrAddr(config.host, ports, "");
    //mars::stn::SetShortlinkSvrAddr(80, ""); // urgly
    std::vector<std::string> backupIps;
    backupIps.push_back(config.backupIp);
    mars::stn::SetBackupIPs(config.host, backupIps);
    //set callback
    mars::stn::SetCallback(marsPlus::StnCallBack::Instance());
    #ifndef ANDROID
        mars::app::SetCallback(marsPlus::AppCallBack::Instance());
    #endif
    //oncreate
    mars::baseevent::OnCreate();
    //程序前后台改变时调用，必须调用，否则可能会出现网络连接频率没那么快的问题
    mars::baseevent::OnForeground(true);
    //检测长链接状态。如果没有连接上，则会尝试重连
    mars::stn::MakesureLonglinkConnected();

    return 0;
}

int marsPlus::sendMessage(const char * body, const uint32_t size, const ResponseListener * callback)
{
    mars::stn::Task task;
    //uint32_t msgId = ImMessage::newMessageId();
    ImMessage* msg=new ImMessage(body, size);
    task.taskid = msg->getMessageId();
    task.cmdid = 55;
    task.channel_select = mars::stn::Task::kChannelLong;
    task.need_authed = 0;
    task.priority = mars::stn::Task::kTaskPriorityNormal;
    xdebug2(TSF"sendMsg taskid:%0,cmdid:%1", task.taskid, task.cmdid);
    //save task context
    TaskContext *ctx = new TaskContext();
    ctx->respCallback = (void*)callback;
    ctx->msg = msg; // to be released in removeTaskConext
    ctx->size = size+sizeof(ImMessage::MessageFormat);
    MarsTaskProxy::storeContext(task.taskid, ctx);

    mars::stn::StartTask(task);
    return 0;

}

void marsPlus::registerPushListener( PushListener * const listener)
{
    g_pushListener = listener;
}

void marsPlus::registerChannelListener( ChannelListener * const listener)
{
    g_channelListener = listener;
}
