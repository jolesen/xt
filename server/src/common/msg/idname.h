// This file is auto generated.
#ifndef _COMMON_MSG_IDNAME_H_
#define _COMMON_MSG_IDNAME_H_

#include "../core/core.h"

class CMsgIdName
{
public:
	std::string GetMsgName(uint id) { return mIdName[id]; }
	uint GetMsgId(const std::string &name) { return mNameId[name]; }

private:
	UIntStringMap mIdName;
	StringUIntMap mNameId;

	friend CMsgIdName& CSingleton<CMsgIdName>::Instance();
	~CMsgIdName(){ }
	CMsgIdName()
	{
		mIdName[15088026] = "QSysRegistService";                            mNameId["QSysRegistService"] = 15088026;
		mIdName[17205450] = "RSysRegistService";                            mNameId["RSysRegistService"] = 17205450;
		mIdName[17494275] = "QSysAuthUser";                                 mNameId["QSysAuthUser"] = 17494275;
		mIdName[20431463] = "RSysAuthUser";                                 mNameId["RSysAuthUser"] = 20431463;
		mIdName[17300663] = "QSysLoadUser";                                 mNameId["QSysLoadUser"] = 17300663;
		mIdName[16312969] = "RSysLoadUser";                                 mNameId["RSysLoadUser"] = 16312969;
		mIdName[15782802] = "QSysSaveUser";                                 mNameId["QSysSaveUser"] = 15782802;
		mIdName[19782076] = "QSysBroadcast";                                mNameId["QSysBroadcast"] = 19782076;
		mIdName[16918333] = "QSysUserLogout";                               mNameId["QSysUserLogout"] = 16918333;
		mIdName[17019185] = "RSysUserLogout";                               mNameId["RSysUserLogout"] = 17019185;
		mIdName[18847537] = "QSysSayHello";                                 mNameId["QSysSayHello"] = 18847537;
		mIdName[16306049] = "RSysSayHello";                                 mNameId["RSysSayHello"] = 16306049;
		mIdName[15795049] = "QTestTime";                                    mNameId["QTestTime"] = 15795049;
		mIdName[15533661] = "RTestTime";                                    mNameId["RTestTime"] = 15533661;
		mIdName[19037643] = "QUserModule";                                  mNameId["QUserModule"] = 19037643;
		mIdName[17215044] = "RUserModule";                                  mNameId["RUserModule"] = 17215044;
		mIdName[15335115] = "RUserErrorCode";                               mNameId["RUserErrorCode"] = 15335115;
		mIdName[18252001] = "QUserLogin";                                   mNameId["QUserLogin"] = 18252001;
		mIdName[16664991] = "RUserLogin";                                   mNameId["RUserLogin"] = 16664991;
		mIdName[16767626] = "QUserLogout";                                  mNameId["QUserLogout"] = 16767626;
		mIdName[19294912] = "RUserLogout";                                  mNameId["RUserLogout"] = 19294912;
		mIdName[18696177] = "RMsgTest";                                     mNameId["RMsgTest"] = 18696177;
		mIdName[17628228] = "QServerTime";                                  mNameId["QServerTime"] = 17628228;
		mIdName[17277942] = "RServerTime";                                  mNameId["RServerTime"] = 17277942;
		mIdName[18101475] = "QValueVarList";                                mNameId["QValueVarList"] = 18101475;
		mIdName[17588786] = "RValueVarList";                                mNameId["RValueVarList"] = 17588786;
		mIdName[18342553] = "RValueVarModified";                            mNameId["RValueVarModified"] = 18342553;
		mIdName[18014361] = "QValueTimeList";                               mNameId["QValueTimeList"] = 18014361;
		mIdName[18283410] = "RValueTimeList";                               mNameId["RValueTimeList"] = 18283410;
		mIdName[17986452] = "RValueTimeModified";                           mNameId["RValueTimeModified"] = 17986452;
		mIdName[15138912] = "QValueDailyResetList";                         mNameId["QValueDailyResetList"] = 15138912;
		mIdName[20044644] = "RValueDailyResetList";                         mNameId["RValueDailyResetList"] = 20044644;
		mIdName[19686845] = "RValueDailyResetModified";                     mNameId["RValueDailyResetModified"] = 19686845;
		mIdName[15223480] = "QValueCountList";                              mNameId["QValueCountList"] = 15223480;
		mIdName[19225703] = "RValueCountList";                              mNameId["RValueCountList"] = 19225703;
		mIdName[15457256] = "RValueCountModified";                          mNameId["RValueCountModified"] = 15457256;
	}
};
#define theMsgIdName CSingleton<CMsgIdName>::Instance()

#endif