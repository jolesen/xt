#ifndef _LOGIC_TEST_H_
#define _LOGIC_TEST_H_

// CTester
class CTester
{
public:
    bool Init();

private:
    static bool TestModify(const STimerInfo &info);
};
#define theTester CSingleton<CTester>::Instance()

#endif
