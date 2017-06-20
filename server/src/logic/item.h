#ifndef _LOGIC_ITEM_H_
#define _LOGIC_ITEM_H_

#include "common/msg/user.h"
#include "common/data/item.h"

// item
// 所有的"物品"[kItem类型]操作接口都在这里，各个类型的数据可以存储在不同的模块中
namespace item
{
    struct SItemModule
    {
        typedef void (*FunItemEvent)(SUser &user, uint id, uint count, uint path);
        struct SItemEventHandler
        {
            FunItemEvent funBase;
            std::map<uint, FunItemEvent> funSubs;
        };

        std::map<uint, SItemEventHandler> funAdds;
        std::map<uint, SItemEventHandler> funTakes;
    };
    #define theItemModule CSingleton<SItemModule>::Instance()

    uint Add(SUser &user, const S3UInt &item, uint path);
    uint Add(SUser &user, const S3UIntList &list, uint path);
    uint Take(SUser &user, const S3UInt &item, uint path);
    uint Take(SUser &user, const S3UIntList &list, uint path);
    bool CanTake(SUser &user, const S3UInt &item);

    bool RegisterEvent(uint type, SItemModule::FunItemEvent funAdd, SItemModule::FunItemEvent funTake);
    bool RegisterEventSub(uint type, uint id, SItemModule::FunItemEvent funAdd, SItemModule::FunItemEvent funTake);
}

// ----------------------------------
// 大类kItemXxx事件
#define ITEM_EVENT(T)                                                                                                   \
    struct SEventAdd##T : public SEventBase                                                                             \
    {                                                                                                                   \
        SUser& user;                                                                                                    \
        uint id;                                                                                                        \
        uint count;                                                                                                     \
        uint path;                                                                                                      \
        SEventAdd##T(SUser &u, uint i, uint c, uint p) : user(u), id(i), count(c), path(p) { }                          \
    };                                                                                                                  \
    struct SEventTake##T : public SEventBase                                                                            \
    {                                                                                                                   \
        SUser& user;                                                                                                    \
        uint id;                                                                                                        \
        uint count;                                                                                                     \
        uint path;                                                                                                      \
        SEventTake##T(SUser &u, uint i, uint c, uint p) : user(u), id(i), count(c), path(p) { }                         \
    };                                                                                                                  \
    void DispatchAdd##T (SUser &user, uint id, uint count, uint path) { EVENT(SEventAdd##T,  user, id, count, path); }; \
    void DispatchTake##T(SUser &user, uint id, uint count, uint path) { EVENT(SEventTake##T, user, id, count, path); }; \
    bool regEventItem##T = item::RegisterEvent(kItem##T, DispatchAdd##T, DispatchTake##T);


// 子类kItemXxxYyy事件
#define ITEM_EVENT_SUB(MT, ST)                                                                                                  \
    struct SEventAdd##MT##ST : public SEventBase                                                                                \
    {                                                                                                                           \
        SUser& user;                                                                                                            \
        uint count;                                                                                                             \
        uint path;                                                                                                              \
        SEventAdd##MT##ST(SUser &u, uint c, uint p) : user(u), count(c), path(p) { }                                            \
    };                                                                                                                          \
    struct SEventTake##MT##ST : public SEventBase                                                                               \
    {                                                                                                                           \
        SUser& user;                                                                                                            \
        uint count;                                                                                                             \
        uint path;                                                                                                              \
        SEventTake##MT##ST(SUser &u, uint c, uint p) : user(u), count(c), path(p) { }                                           \
    };                                                                                                                          \
    void DispatchAdd##MT##ST (SUser &user, uint id, uint count, uint path) { EVENT(SEventAdd##MT##ST,  user, count, path); };   \
    void DispatchTake##MT##ST(SUser &user, uint id, uint count, uint path) { EVENT(SEventTake##MT##ST, user, count, path); };   \
    bool regEventItem##MT##ST = item::RegisterEventSub(kItem##MT, kItem##MT##ST, DispatchAdd##MT##ST, DispatchTake##MT##ST);


// 类型事件
ITEM_EVENT(Prop);
ITEM_EVENT(Coin);
ITEM_EVENT_SUB(Coin, Money);
ITEM_EVENT_SUB(Coin, Gold);
ITEM_EVENT(Equip);

#undef ITEM_EVENT
#undef ITEM_SUB_EVENT

#endif
