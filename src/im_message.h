#ifndef __IMMESSAGE_H__
#define __IMMESSAGE_H__
#include <stdint.h>
#include <string>
class ImMessage {
public:
    // Message format used for client/server communication
    // 16bytes header followed with message body
    #pragma pack(push, 1)
    struct MessageFormat
    {
        uint32_t magicNum; // magically defined num for error message checking
        uint32_t messageId; // unqiue message identification
        uint32_t len;       // body length
        char data[];     // body start byte
    };
    #pragma pack(pop)

    ImMessage(const char* data, const int size);
    ImMessage();
    ~ImMessage();
    const uint32_t magic_num = 0xdeadbeaf;
    static uint32_t newMessageId();
    std::string getData();
    char* getMessage();
    uint32_t getMessageSize();
    bool isValid();
    uint32_t getMessageId();
    bool parse(const char* msg, const int size);
    static uint32_t messageId;
private:
    MessageFormat *m_message;

};
#endif // !__IMMESSAGE_H__
