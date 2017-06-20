#ifndef _LOGIC_MODULE_H_
#define _LOGIC_MODULE_H_

#include "common/msg/user.h"
#include "common/data/modules.h"

// module
namespace module
{
    // data
    typedef void (*FunEncodeModule)(const SUser &user, CCoder &coder);
    struct SData
    {
        FunEncodeModule encoders[kModuleMax];
    };
    #define theData CSingleton<SData>::Instance()

    // public
    bool Init();
    void InitMd5(SUser &user);
    bool Register(uint module, FunEncodeModule fun_coder);
    void RespondModule(const SUser &user, uint module);
}

#endif
