#ifndef _COMMON_MSG_IDNAME_H_
#define _COMMON_MSG_IDNAME_H_

#include "../core/core.h"

class CMsgIdName
{
public:
	std::string GetMsgName(uint id) { return m_idName[id]; }
	uint GetMsgId(const std::string &name) { return m_nameId[name]; }

private:
	UIntStringMap m_idName;
	StringUIntMap m_nameId;

	friend CMsgIdName& CSingleton<CMsgIdName>::Instance();
	~CMsgIdName(){ }
	CMsgIdName()
	{
		m_idName[15088026] = "QSysRegistService";                           m_nameId["QSysRegistService"] = 15088026;
		m_idName[17205450] = "RSysRegistService";                           m_nameId["RSysRegistService"] = 17205450;
		m_idName[17494275] = "QSysAuthUser";                                m_nameId["QSysAuthUser"] = 17494275;
		m_idName[20431463] = "RSysAuthUser";                                m_nameId["RSysAuthUser"] = 20431463;
		m_idName[17300663] = "QSysLoadUser";                                m_nameId["QSysLoadUser"] = 17300663;
		m_idName[16312969] = "RSysLoadUser";                                m_nameId["RSysLoadUser"] = 16312969;
		m_idName[15782802] = "QSysSaveUser";                                m_nameId["QSysSaveUser"] = 15782802;
		m_idName[19782076] = "QSysBroadcast";                               m_nameId["QSysBroadcast"] = 19782076;
		m_idName[16918333] = "QSysUserLogout";                              m_nameId["QSysUserLogout"] = 16918333;
		m_idName[17019185] = "RSysUserLogout";                              m_nameId["RSysUserLogout"] = 17019185;
		m_idName[18847537] = "QSysSayHello";                                m_nameId["QSysSayHello"] = 18847537;
		m_idName[16306049] = "RSysSayHello";                                m_nameId["RSysSayHello"] = 16306049;
		m_idName[15795049] = "QTestTime";                                   m_nameId["QTestTime"] = 15795049;
		m_idName[15533661] = "RTestTime";                                   m_nameId["RTestTime"] = 15533661;
		m_idName[19037643] = "QUserModule";                                 m_nameId["QUserModule"] = 19037643;
		m_idName[17215044] = "RUserModule";                                 m_nameId["RUserModule"] = 17215044;
		m_idName[15335115] = "RUserErrorCode";                              m_nameId["RUserErrorCode"] = 15335115;
		m_idName[18252001] = "QUserLogin";                                  m_nameId["QUserLogin"] = 18252001;
		m_idName[16664991] = "RUserLogin";                                  m_nameId["RUserLogin"] = 16664991;
		m_idName[16767626] = "QUserLogout";                                 m_nameId["QUserLogout"] = 16767626;
		m_idName[19294912] = "RUserLogout";                                 m_nameId["RUserLogout"] = 19294912;
		m_idName[18696177] = "RMsgTest";                                    m_nameId["RMsgTest"] = 18696177;
		m_idName[17628228] = "QServerTime";                                 m_nameId["QServerTime"] = 17628228;
		m_idName[17277942] = "RServerTime";                                 m_nameId["RServerTime"] = 17277942;
		m_idName[18101475] = "QValueVarList";                               m_nameId["QValueVarList"] = 18101475;
		m_idName[17588786] = "RValueVarList";                               m_nameId["RValueVarList"] = 17588786;
		m_idName[18342553] = "RValueVarModified";                           m_nameId["RValueVarModified"] = 18342553;
		m_idName[18014361] = "QValueTimeList";                              m_nameId["QValueTimeList"] = 18014361;
		m_idName[18283410] = "RValueTimeList";                              m_nameId["RValueTimeList"] = 18283410;
		m_idName[17986452] = "RValueTimeModified";                          m_nameId["RValueTimeModified"] = 17986452;
		m_idName[15138912] = "QValueDailyResetList";                        m_nameId["QValueDailyResetList"] = 15138912;
		m_idName[20044644] = "RValueDailyResetList";                        m_nameId["RValueDailyResetList"] = 20044644;
		m_idName[19686845] = "RValueDailyResetModified";                    m_nameId["RValueDailyResetModified"] = 19686845;
		m_idName[15223480] = "QValueCountList";                             m_nameId["QValueCountList"] = 15223480;
		m_idName[19225703] = "RValueCountList";                             m_nameId["RValueCountList"] = 19225703;
		m_idName[15457256] = "RValueCountModified";                         m_nameId["RValueCountModified"] = 15457256;
	}
};
#define theMsgIdName CSingleton<CMsgIdName>::Instance()

#endif