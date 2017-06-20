#include "user.h"

//SUserBaseInfo
void SUserBaseInfo::Encode(CCoder &coder) const
{
	coder << id;
	coder << time;
	coder << session;
	coder << day_reset;
}
void SUserBaseInfo::Decode(CCoder &coder)
{
	coder >> id;
	coder >> time;
	coder >> session;
	coder >> day_reset;
}

//SUserInner
void SUserInner::Encode(CCoder &coder) const
{
	coder << last_save;
	coder << md5_list;
	coder << dirty_list;
	coder << hh_test;
}
void SUserInner::Decode(CCoder &coder)
{
	coder >> last_save;
	coder >> md5_list;
	coder >> dirty_list;
	coder >> hh_test;
}

//SUser
void SUser::Encode(CCoder &coder) const
{
	base.Encode(coder);
	times.Encode(coder);
	vars.Encode(coder);
	counts.Encode(coder);
	daily_resets.Encode(coder);
	inner.Encode(coder);
	item.Encode(coder);
}
void SUser::Decode(CCoder &coder)
{
	base.Decode(coder);
	times.Decode(coder);
	vars.Decode(coder);
	counts.Decode(coder);
	daily_resets.Decode(coder);
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
	//test_map_i
	coder << (uint)test_map_i.size();
	for(std::map<uint, SDailyResets>::const_iterator iter = test_map_i.begin(); iter != test_map_i.end(); ++iter)
	{
		coder << iter->first;
		iter->second.Encode(coder);
		if(coder.HasError()) { break; }
	}
	coder << ivalue;
	coder << vvalue;
	//test_map_s
	coder << (uint)test_map_s.size();
	for(std::map<std::string, SDailyResets>::const_iterator iter = test_map_s.begin(); iter != test_map_s.end(); ++iter)
	{
		coder << iter->first;
		iter->second.Encode(coder);
		if(coder.HasError()) { break; }
	}
}
void QUserLogout::DecodeBody(CCoder &coder)
{
	//test_map_i
	test_map_i.clear();
	uint len_0 = 0;
	coder >> len_0;
	FOR(len_0, i)
	{
		uint key;
		SDailyResets value;
		coder >> key;
		value.Decode(coder);
		if(coder.HasError()) { break; }
		test_map_i[key] = value;
	}
	coder >> ivalue;
	coder >> vvalue;
	//test_map_s
	test_map_s.clear();
	uint len_3 = 0;
	coder >> len_3;
	FOR(len_3, i)
	{
		std::string key;
		SDailyResets value;
		coder >> key;
		value.Decode(coder);
		if(coder.HasError()) { break; }
		test_map_s[key] = value;
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
