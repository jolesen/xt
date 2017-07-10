// This file is auto generated.
#ifndef _COMMON_MSG_SYSTEM_H_
#define _COMMON_MSG_SYSTEM_H_

#include "user.h"

// SService
struct SService
{
	string sid;
	string ip;
	uint port;

	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

// QSysRegistService
class QSysRegistService : public CMsgBase
{
public:
	QSysRegistService() { header.type = kMsgTypeSystem; header.id = 15088026; }
	~QSysRegistService(){ }

public:
	SService service;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// RSysRegistService
class RSysRegistService : public CMsgBase
{
public:
	RSysRegistService() { header.type = kMsgTypeSystem; header.id = 17205450; }
	~RSysRegistService(){ }

public:
	bool flag;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// QSysAuthUser
class QSysAuthUser : public CMsgBase
{
public:
	QSysAuthUser() { header.type = kMsgTypeSystem; header.id = 17494275; }
	~QSysAuthUser(){ }

public:
	uint uid;
	uint session;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// RSysAuthUser
class RSysAuthUser : public CMsgBase
{
public:
	RSysAuthUser() { header.type = kMsgTypeSystem; header.id = 20431463; }
	~RSysAuthUser(){ }

public:
	bool authed;
	uint uid;
	uint session;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// QSysLoadUser
class QSysLoadUser : public CMsgBase
{
public:
	QSysLoadUser() { header.type = kMsgTypeSystem; header.id = 17300663; }
	~QSysLoadUser(){ }

public:
	uint uid;
	uint session;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// RSysLoadUser
class RSysLoadUser : public CMsgBase
{
public:
	RSysLoadUser() { header.type = kMsgTypeSystem; header.id = 16312969; }
	~RSysLoadUser(){ }

public:
	uint flag;
	uint uid;
	uint session;
	SUser user;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// QSysSaveUser
class QSysSaveUser : public CMsgBase
{
public:
	QSysSaveUser() { header.type = kMsgTypeSystem; header.id = 15782802; }
	~QSysSaveUser(){ }

public:
	uint uid;
	std::map<uint, SBytes> modules;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// QSysBroadcast
class QSysBroadcast : public CMsgBase
{
public:
	QSysBroadcast() { header.type = kMsgTypeSystem; header.id = 19782076; }
	~QSysBroadcast(){ }

public:
	UIntList guids;
	SBytes msg;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// QSysUserLogout
class QSysUserLogout : public CMsgBase
{
public:
	QSysUserLogout() { header.type = kMsgTypeSystem; header.id = 16918333; }
	~QSysUserLogout(){ }

public:
	uint uid;
	uint reason;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// RSysUserLogout
class RSysUserLogout : public CMsgBase
{
public:
	RSysUserLogout() { header.type = kMsgTypeSystem; header.id = 17019185; }
	~RSysUserLogout(){ }

public:
	uint uid;
	uint reason;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// QSysSayHello
class QSysSayHello : public CMsgBase
{
public:
	QSysSayHello() { header.type = kMsgTypeSystem; header.id = 18847537; }
	~QSysSayHello(){ }

public:
	UIntList list;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// RSysSayHello
class RSysSayHello : public CMsgBase
{
public:
	RSysSayHello() { header.type = kMsgTypeSystem; header.id = 16306049; }
	~RSysSayHello(){ }

protected:
	void EncodeBody(CCoder &coder) const{ }
	void DecodeBody(CCoder &coder){ }
};

#endif