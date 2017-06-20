#include "common/core/event.h"
#include "common/core/util.h"
#include "common/data/item_const.h"
#include "common/misc.h"
#include "item.h"

namespace item
{

uint Add(SUser &user, const S3UInt &item, uint path)
{
    S3UIntList list;
    list.push_back(item);

    return Add(user, list, path);
}

uint Add(SUser &user, const S3UIntList &list, uint path)
{
    FORLIST(list, i)
    {
        const S3UInt &item = list[i];
        if((item.type == 0) || (item.type > kItemMax))
        {
            return kErrorItemUnknownType;
        }

        // 事件
        IF_FIND(theItemModule.funAdds, item.type, iter)
        {
            iter->second.funBase(user, item.id, item.value, path);

            IF_FIND(iter->second.funSubs, item.id, iter_sub)
            {
                iter_sub->second(user, item.id, item.value, path);
            }
        }
    }

    return 0;
}

uint Take(SUser &user, const S3UInt &item, uint path)
{
    return 0;
}

uint Take(SUser &user, const S3UIntList &list, uint path)
{
    return 0;
}

bool CanTake(SUser &user, const S3UInt &item)
{
    return false;
}

bool RegisterEvent(uint type, SItemModule::FunItemEvent funAdd, SItemModule::FunItemEvent funTake)
{
    theItemModule.funAdds[type].funBase  = funAdd;
    theItemModule.funTakes[type].funBase = funTake;
    return true;
}

bool RegisterEventSub(uint type, uint id, SItemModule::FunItemEvent funAdd, SItemModule::FunItemEvent funTake)
{
    theItemModule.funAdds[type].funSubs[id]  = funAdd;
    theItemModule.funTakes[type].funSubs[id] = funTake;
    return true;
}

}
