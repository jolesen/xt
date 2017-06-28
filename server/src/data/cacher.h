#ifndef _DATA_CACHER_H_
#define _DATA_CACHER_H_

#include "dbio.h"

// CCacher
class CCacher
{
public:
    typedef std::map<uint, SUser*> UserMap;

public:
    CCacher() { }
    ~CCacher(){ }

    bool Init();
    void Start();
    void Load(uint uid, uint session);
    void Save(uint uid, const std::map<uint, SBytes> &modules);
    void Remove(uint uid);
    void CheckLoaded();
    SUser* Find(uint uid);

private:
    CLoader mLoader;
    CSaver  mSaver;
    UserMap mUsers;

    static CCacher* mInstance;
};
#define theCacher CSingleton<CCacher>::Instance()

#endif
