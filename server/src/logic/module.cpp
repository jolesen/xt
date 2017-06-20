#include "common/core/sconfig.h"
#include "common/core/server.h"
#include "common/core/timer.h"
#include "common/core/time.h"
#include "common/core/log.h"
#include "common/core/md5.h"
#include "common/msg/system.h"
#include "common/misc.h"
#include "module.h"
#include "define.h"
#include "user.h"

// ----------------------------------------
#define MODULE(M, T)\
    void EncodeModule_##M(const SUser &user, CCoder &coder) { user.T.Encode(coder); };\
    bool reg_module_encoder_##M = module::Register(M, EncodeModule_##M);

MODULE(kModuleDailyReset, daily_resets);
MODULE(kModuleVar,        vars);
MODULE(kModuleTime,       times);
MODULE(kModuleCount,      counts);

// ----------------------------------------
namespace module
{

bool CheckModuleDirty(const STimerInfo &info)
{
    QSysSaveUser req;
    SBytes data;

    uint now = CTime::Now();
    user::UserMap &users = user::GetUsers();
    std::string str_md5;
    FORMAP(users, iter)
    {
        SUser &user = iter->second;
        // 按照玩家的登陆时间为起点，每隔n秒保存数据，这样就可以错开一次性发送大量的数据包的情况
        // TODO 初期最好做一个定期检测的机制，看是否这样的方法是否会漏掉没有保存的数据
        if(now - user.inner.last_save >= USER_SAVE_INTERVAL)
        {
            user.inner.last_save = now;

            FOR(kModuleMax, i)
            {
                FunEncodeModule &fp_encoder = theData.encoders[i];
                if(fp_encoder)
                {
                    CCoder coder(data.bytes, sizeof(data.bytes));
                    fp_encoder(user, coder);
                    data.size = coder.GetCodedSize();

                    MD5 md5; // 如何优化，把这个放到最外层
                    md5.update(data.bytes, data.size);
                    md5.finalize();

                    str_md5 = md5.hexdigest();
                    if(user.inner.md5_list[i] != str_md5)
                    {
                        user.inner.md5_list[i] = str_md5;
                        req.modules[i] = data;
                    }
                }
            }

            if(req.modules.size() > 0)
            {
                req.uid = user.base.id;
                //SEND_DATA(req);

                req.modules.clear();
            }
        }
    }

    return true;
}

bool Init()
{
    uint timer = theTimer.AddSecond(10, 0, CheckModuleDirty);
    return (timer != 0);
}

void InitMd5(SUser &user)
{
    char buff[MSG_MAX];
    FOR(kModuleMax, i)
    {
        FunEncodeModule &fp_encoder = theData.encoders[i];
        if(fp_encoder)
        {
            CCoder coder(buff, sizeof(buff));
            fp_encoder(user, coder);

            MD5 md5;
            md5.update(buff, coder.GetCodedSize());
            md5.finalize();

            user.inner.md5_list[i] = md5.hexdigest();
        }
    }

    user.inner.last_save = CTime::Now();
}

void RespondModule(const SUser &user, uint module)
{
    if((module < kModuleMax) && theData.encoders[module])
    {
        RUserModule rsp;
        rsp.module = module;

        // encode module
        CCoder coder(rsp.data.bytes, sizeof(rsp.data.bytes));
        theData.encoders[module](user, coder);
        rsp.data.size = coder.GetCodedSize();

        SEND_USER(user, rsp);
    }
    else
    {
        LOG_ERROR("module=%u, kModuleMax=%u, or no coder", module, kModuleMax);
    }
}

bool Register(uint module, FunEncodeModule fun_coder)
{
    if(module < kModuleMax)
    {
        theData.encoders[module] = fun_coder;
    }

    return true;
}

}

// -------------------------------------------------------
MSG_USER(QUserModule)
{
    module::RespondModule(user, msg.module);
}
