#ifndef _COMMON_MSG_USER_H_
#define _COMMON_MSG_USER_H_

#include "item.h"
#include "value.h"

// SUserBaseInfo
struct SUserBaseInfo
{
public:
	uint id;
	uint time;
	uint session;
	uint day_reset;

public:
	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

// SUserInner
struct SUserInner
{
public:
	uint last_save;
	UIntStringMap md5_list;
	UIntBoolMap dirty_list;
	UIntList hh_test;

public:
	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

// SUser
struct SUser
{
public:
	SUserBaseInfo base;
	STimes times;
	SVars vars;
	SCounts counts;
	SDailyResets daily_resets;
	SUserInner inner;
	SItem item;

public:
	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

// QUserModule
class QUserModule : public CMsgBase
{
public:
	QUserModule() { header.type = kMsgTypeClient; header.id = 19037643; }
	~QUserModule(){ }

public:
	uint module;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// RUserModule
class RUserModule : public CMsgBase
{
public:
	RUserModule() { header.type = kMsgTypeClient; header.id = 17215044; }
	~RUserModule(){ }

public:
	uint module;
	SBytes data;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// RUserErrorCode
class RUserErrorCode : public CMsgBase
{
public:
	RUserErrorCode() { header.type = kMsgTypeClient; header.id = 15335115; }
	~RUserErrorCode(){ }

public:
	uint code;
	UIntList flags;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// QUserLogin
class QUserLogin : public CMsgBase
{
public:
	QUserLogin() { header.type = kMsgTypeClient; header.id = 18252001; }
	~QUserLogin(){ }

public:
	uint uid;
	uint session;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// RUserLogin
class RUserLogin : public CMsgBase
{
public:
	RUserLogin() { header.type = kMsgTypeClient; header.id = 16664991; }
	~RUserLogin(){ }

public:
	uint flag;
	uint uid;
	uint session;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// QUserLogout
class QUserLogout : public CMsgBase
{
public:
	QUserLogout() { header.type = kMsgTypeClient; header.id = 16767626; }
	~QUserLogout(){ }

public:
	std::map<uint, SDailyResets> test_map_i;
	uint ivalue;
	UIntList vvalue;
	std::map<std::string, SDailyResets> test_map_s;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// RUserLogout
class RUserLogout : public CMsgBase
{
public:
	RUserLogout() { header.type = kMsgTypeClient; header.id = 19294912; }
	~RUserLogout(){ }

public:
	uint flag;
	StringList vtest;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// RMsgTest
class RMsgTest : public CMsgBase
{
public:
	RMsgTest() { header.type = kMsgTypeClient; header.id = 18696177; }
	~RMsgTest(){ }

public:
	UIntUIntMap test;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// QServerTime
class QServerTime : public CMsgBase
{
public:
	QServerTime() { header.type = kMsgTypeClient; header.id = 17628228; }
	~QServerTime(){ }

protected:
	void EncodeBody(CCoder &coder) const{ }
	void DecodeBody(CCoder &coder){ }
};

// RServerTime
class RServerTime : public CMsgBase
{
public:
	RServerTime() { header.type = kMsgTypeClient; header.id = 17277942; }
	~RServerTime(){ }

public:
	uint time;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

#endif