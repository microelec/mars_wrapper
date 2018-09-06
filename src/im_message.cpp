#include "im_message.h"
#include <vector>
uint32_t ImMessage::messageId = 10000;
ImMessage::ImMessage(const char * data, const int size)
{
    uint32_t msgSize = sizeof(ImMessage::MessageFormat) + size;
    m_message = (ImMessage::MessageFormat*)malloc(msgSize);
    m_message->len = size;
    m_message->magicNum = magic_num;
    m_message->messageId = newMessageId();
    memcpy(m_message->data, data, size);
}

ImMessage::ImMessage()
{
    m_message = NULL;
}

ImMessage::~ImMessage()
{
    if (m_message != NULL) {
        free(m_message);
        m_message = NULL;
    }
}

uint32_t ImMessage::newMessageId()
{
    return ImMessage::messageId++;
}

std::string ImMessage::getData()
{
    return std::string((char *)m_message->data,m_message->len);
}

char * ImMessage::getMessage()
{
    return (char*)m_message;
}

uint32_t ImMessage::getMessageSize()
{
    return m_message->len+sizeof(MessageFormat);
}

bool ImMessage::isValid()
{
    return m_message->magicNum==magic_num;
}

uint32_t ImMessage::getMessageId()
{
    return m_message->messageId;
}

bool ImMessage::parse(const char * msg, const int size)
{
    if (m_message != NULL) return false;
    m_message = (ImMessage::MessageFormat*)malloc(size);
    memcpy(m_message, msg, size);
   
    return (m_message->magicNum == magic_num)?true:false;
}
