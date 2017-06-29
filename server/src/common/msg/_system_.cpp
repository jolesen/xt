// This file is auto generated.
#include "system.h"

//SService
void SService::Encode(CCoder &coder) const
{
	coder << sid;
	coder << ip;
	coder << port;
}
void SService::Decode(CCoder &coder)
{
	coder >> sid;
	coder >> ip;
	coder >> port;
}

//QSysRegistService
void QSysRegistService::EncodeBody(CCoder &coder) const
{
	service.Encode(coder);
}
void QSysRegistService::DecodeBody(CCoder &coder)
{
	service.Decode(coder);
}

//RSysRegistService
void RSysRegistService::EncodeBody(CCoder &coder) const
{
	coder << flag;
}
void RSysRegistService::DecodeBody(CCoder &coder)
{
	coder >> flag;
}

//QSysAuthUser
void QSysAuthUser::EncodeBody(CCoder &coder) const
{
	coder << uid;
	coder << session;
}
void QSysAuthUser::DecodeBody(CCoder &coder)
{
	coder >> uid;
	coder >> session;
}

//RSysAuthUser
void RSysAuthUser::EncodeBody(CCoder &coder) const
{
	coder << authed;
	coder << uid;
	coder << session;
}
void RSysAuthUser::DecodeBody(CCoder &coder)
{
	coder >> authed;
	coder >> uid;
	coder >> session;
}

//QSysLoadUser
void QSysLoadUser::EncodeBody(CCoder &coder) const
{
	coder << uid;
	coder << session;
}
void QSysLoadUser::DecodeBody(CCoder &coder)
{
	coder >> uid;
	coder >> session;
}

//RSysLoadUser
void RSysLoadUser::EncodeBody(CCoder &coder) const
{
	coder << flag;
	coder << uid;
	coder << session;
	user.Encode(coder);
}
void RSysLoadUser::DecodeBody(CCoder &coder)
{
	coder >> flag;
	coder >> uid;
	coder >> session;
	user.Decode(coder);
}

//QSysSaveUser
void QSysSaveUser::EncodeBody(CCoder &coder) const
{
	coder << uid;
	//modules
	coder << (uint)modules.size();
	FORMAP(modules, iter)
	{
		coder << iter->first;
		coder << iter->second;
		if(coder.HasError()) { break; }
	}
}
void QSysSaveUser::DecodeBody(CCoder &coder)
{
	coder >> uid;
	//modules
	modules.clear();
	uint len1 = 0;
	coder >> len1;
	FOR(len1, i)
	{
		uint key;
		SBytes value;
		coder >> key;
		coder >> value;
		if(coder.HasError()) { break; }
		modules[key] = value;
	}
}

//QSysBroadcast
void QSysBroadcast::EncodeBody(CCoder &coder) const
{
	coder << guids;
	coder << msg;
}
void QSysBroadcast::DecodeBody(CCoder &coder)
{
	coder >> guids;
	coder >> msg;
}

//QSysUserLogout
void QSysUserLogout::EncodeBody(CCoder &coder) const
{
	coder << uid;
	coder << reason;
}
void QSysUserLogout::DecodeBody(CCoder &coder)
{
	coder >> uid;
	coder >> reason;
}

//RSysUserLogout
void RSysUserLogout::EncodeBody(CCoder &coder) const
{
	coder << uid;
	coder << reason;
}
void RSysUserLogout::DecodeBody(CCoder &coder)
{
	coder >> uid;
	coder >> reason;
}

//QSysSayHello
void QSysSayHello::EncodeBody(CCoder &coder) const
{
	coder << list;
}
void QSysSayHello::DecodeBody(CCoder &coder)
{
	coder >> list;
}
