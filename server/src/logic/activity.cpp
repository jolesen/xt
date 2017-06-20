#include "common/core/event.h"
#include "common/core/log.h"
#include "activity.h"
#include "common/data/activity_const.h"
#include "login.h"

// activity
namespace activity
{

void HandleAction(SUser &user, uint action)
{
    FORMAP(theData.actions, iter)
    {
        const CActivityData::Type *cfg = theActivityData.Find(iter->first);
        if(!cfg)
        {
            LOG_ERROR("no activity[id=%u] cfg", iter->first);
            continue;
        }

        FORMAP(iter->second, iter_action)
        {
            if(iter_action->first == action)
            {
                iter_action->second(user, *cfg);
                //LOG_DEBUG("cfg_id=%u, action=%u", iter->first, iter_action->first);
            }
        }
    }
}

bool RegisterAction(uint id, uint action, FunActivityAction fun_action)
{
    IF_NFIND(theData.actions, id, iter)
    {
        theData.actions[id][action] = fun_action;
    }
    else
    {
        (iter->second)[action] = fun_action;
    }

    return true;
}

}

// EVENT
EVENT_FUNC(SEventLogin)
{
    activity::HandleAction(ev.user, kActivityLogin);
}

EVENT_FUNC(SEventLogout)
{
    activity::HandleAction(ev.user, kActivityLogout);
}
