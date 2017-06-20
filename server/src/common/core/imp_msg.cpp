#include "msg.h"

// CMsgBase
CMsgBase::CMsgBase()
{
}

CMsgBase::~CMsgBase()
{
}

void CMsgBase::Encode(CCoder &coder) const
{
    // header
    coder << header.size;
    coder << header.id;
    if(header.type == kMsgTypeSystem)
    {
        coder << header.src_sid;
        coder << header.dst_sid;
    }

    // body
    EncodeBody(coder);

    // reset header size
    SMsgHeader::SetSize(coder.GetBuffer(), coder.GetCodedSize());
}

void CMsgBase::Decode(CCoder &coder)
{
    // header
    coder >> header.size;
    coder >> header.id;
    if(header.type == kMsgTypeSystem)
    {
        coder >> header.src_sid;
        coder >> header.dst_sid;
    }

    // body
    DecodeBody(coder);
}

// CMsgs
CMsgs::CMsgs()
{
}

CMsgs::~CMsgs()
{
}

bool CMsgs::Register(CMsgBase* msg, FunMsg executor, const std::string &name)
{
    if(msg)
    {
        m_funs[msg->header.id]  = executor;
        m_msgs[msg->header.id]  = msg;
    }

    return true;
}

CMsgBase* CMsgs::Create(uint id)
{
    IF_FIND(m_msgs, id, iter)
    {
        return iter->second;
    }

    return NULL;
}

void CMsgs::Execute(const CMsgBase *msg)
{
    if(msg)
    {
        IF_FIND(m_funs, msg->header.id, iter)
        {
            (iter->second)(msg);
        }
    }
}
