#ifndef _LOGIC_ACTIVITY_H_
#define _LOGIC_ACTIVITY_H_

#include "common/data/activity.h"
#include "common/msg/user.h"

// activity
namespace activity
{
    // data
    typedef void (*FunActivityAction)(SUser &user, const CActivityData::Type &type);
    struct SData
    {
        std::map<uint, std::map<uint, FunActivityAction> > actions; // key - activity id
    };
    #define theData CSingleton<SData>::Instance()

    // public
    void HandleAction(SUser &user, uint action);
    bool RegisterAction(uint id, uint action, FunActivityAction funAction);
}

// ACTIVITY_FUNC(ID - acitvity id, A - action)
#define ACTIVITY_FUNC(ID, A)                                                                \
    void OnActivity_##ID##_##A(SUser &user, const CActivityData::Type &type);               \
    bool gRegActivity_##ID##_##A = activity::RegisterAction(ID, A, OnActivity_##ID##_##A);  \
    void OnActivity_##ID##_##A(SUser &user, const CActivityData::Type &type)

#endif
