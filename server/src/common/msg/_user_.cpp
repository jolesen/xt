// This file is auto generated.
#include "user.h"

//SUserBaseInfo
void SUserBaseInfo::Encode(CCoder &coder) const
{
	coder << id;
	coder << time;
	coder << session;
	coder << dayReset;
}
void SUserBaseInfo::Decode(CCoder &coder)
{
	coder >> id;
	coder >> time;
	coder >> session;
	coder >> dayReset;
}

//SUserInner
void SUserInner::Encode(CCoder &coder) const
{
	coder << lastSave;
	coder << md5List;
	coder << dirtyList;
	coder << hhTest;
}
void SUserInner::Decode(CCoder &coder)
{
	coder >> lastSave;
	coder >> md5List;
	coder >> dirtyList;
	coder >> hhTest;
}

//SUser
void SUser::Encode(CCoder &coder) const
{
	base.Encode(coder);
	times.Encode(coder);
	vars.Encode(coder);
	counts.Encode(coder);
	dailyResets.Encode(coder);
	inner.Encode(coder);
	item.Encode(coder);
}
void SUser::Decode(CCoder &coder)
{
	base.Decode(coder);
	times.Decode(coder);
	vars.Decode(coder);
	counts.Decode(coder);
	dailyResets.Decode(coder);
	inner.Decode(coder);
	item.Decode(coder);
}

//QUserModule
void QUserModule::EncodeBody(CCoder &coder) const
{
	coder << module;
}
void QUserModule::DecodeBody(CCoder &coder)
{
	coder >> module;
}

//RUserModule
void RUserModule::EncodeBody(CCoder &coder) const
{
	coder << module;
	coder << data;
}
void RUserModule::DecodeBody(CCoder &coder)
{
	coder >> module;
	coder >> data;
}

//RUserErrorCode
void RUserErrorCode::EncodeBody(CCoder &coder) const
{
	coder << code;
	coder << flags;
}
void RUserErrorCode::DecodeBody(CCoder &coder)
{
	coder >> code;
	coder >> flags;
}

//QUserLogin
void QUserLogin::EncodeBody(CCoder &coder) const
{
	coder << uid;
	coder << session;
}
void QUserLogin::DecodeBody(CCoder &coder)
{
	coder >> uid;
	coder >> session;
}

//RUserLogin
void RUserLogin::EncodeBody(CCoder &coder) const
{
	coder << flag;
	coder << uid;
	coder << session;
}
void RUserLogin::DecodeBody(CCoder &coder)
{
	coder >> flag;
	coder >> uid;
	coder >> session;
}

//QUserLogout
void QUserLogout::EncodeBody(CCoder &coder) const
{
	//testMapI
	coder << (uint)testMapI.size();
	FORMAP(testMapI, iter)
	{
		coder << iter->first;
		iter->second.Encode(coder);
		if(coder.HasError()) { break; }
	}
	coder << ivalue;
	coder << vvalue;
	//testMapS
	coder << (uint)testMapS.size();
	FORMAP(testMapS, iter)
	{
		coder << iter->first;
		iter->second.Encode(coder);
		if(coder.HasError()) { break; }
	}
}
void QUserLogout::DecodeBody(CCoder &coder)
{
	//testMapI
	testMapI.clear();
	uint len0 = 0;
	coder >> len0;
	FOR(len0, i)
	{
		uint key;
		SDailyResets value;
		coder >> key;
		value.Decode(coder);
		if(coder.HasError()) { break; }
		testMapI[key] = value;
	}
	coder >> ivalue;
	coder >> vvalue;
	//testMapS
	testMapS.clear();
	uint len3 = 0;
	coder >> len3;
	FOR(len3, i)
	{
		string key;
		SDailyResets value;
		coder >> key;
		value.Decode(coder);
		if(coder.HasError()) { break; }
		testMapS[key] = value;
	}
}

//RUserLogout
void RUserLogout::EncodeBody(CCoder &coder) const
{
	coder << flag;
	coder << vtest;
}
void RUserLogout::DecodeBody(CCoder &coder)
{
	coder >> flag;
	coder >> vtest;
}

//RMsgTest
void RMsgTest::EncodeBody(CCoder &coder) const
{
	coder << test;
}
void RMsgTest::DecodeBody(CCoder &coder)
{
	coder >> test;
}

//RServerTime
void RServerTime::EncodeBody(CCoder &coder) const
{
	coder << time;
}
void RServerTime::DecodeBody(CCoder &coder)
{
	coder >> time;
}
