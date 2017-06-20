#ifndef _COMMON_CORE_MSG_H_
#define _COMMON_CORE_MSG_H_

#include "coder.h"

const uint kMsgTypeClient = 1;
const uint kMsgTypeSystem = 2;

// SMsgHeader
struct SMsgHeader
{
    uint size;
    uint id;

    // 内部消息
    std::string src_sid;
    std::string dst_sid;

    // 辅助参数，不参与编码
    uint type; // 消息类型
    uint fd;

    static uint GetMinSize() { return sizeof(size) + sizeof(id); }
    static uint ParseSize(const char* buff) { return *((uint*)buff); }
    static uint ParseId(const char* buff) { return *((uint*)(buff + sizeof(uint))); }
    static void SetSize(char* buff, uint value) { *((uint*)buff) = value; }
};

// CMsgBase
class CMsgBase
{
public:
    CMsgBase();
    ~CMsgBase();

    void Encode(CCoder &coder) const;
    void Decode(CCoder &coder);

public:
    SMsgHeader header;

protected:
    virtual void EncodeBody(CCoder &coder) const = 0;
    virtual void DecodeBody(CCoder &coder)       = 0;
};

// CMsgs
class CMsgs
{
public:
    typedef void (*FunMsg)(const CMsgBase *msg);

    CMsgBase* Create(uint id);
    void Execute(const CMsgBase *msg);
    bool Register(CMsgBase* msg, FunMsg executor, const std::string &name);

private:
    friend CMsgs& CSingleton<CMsgs>::Instance();
    CMsgs();
    ~CMsgs();

private:
    std::unordered_map<uint, CMsgBase*> m_msgs;
    std::unordered_map<uint, FunMsg>    m_funs;
};
#define theMsgs CSingleton<CMsgs>::Instance()

// MSG_FUNC
#define MSG_FUNC(T)                                                             \
    T *gMsg##T = new T;                                                         \
    void OnMsg##T(const T& msg);                                                \
    void CastMsg##T(const CMsgBase* msg) { OnMsg##T(*((const T*)msg)); }        \
    bool gRegMsg##T = theMsgs.Register((CMsgBase*)(gMsg##T), CastMsg##T, #T);   \
    void OnMsg##T(const T& msg)

#endif
