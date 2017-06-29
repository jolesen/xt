// This file is auto generated.
#ifndef _COMMON_DATA_ACTIVITY_COPY_TEST_H_
#define _COMMON_DATA_ACTIVITY_COPY_TEST_H_

#include "../core/core.h"

// 活动.xls#activity_copy_test
class CActivityCopyTestData
{
public:
	struct Type
	{
		uint id;
		uint value;
		std::string name;
		uint cycle;
	};
	typedef std::map<uint, Type*>   DataMap;
	typedef std::map<uint, DataMap> DataMapMap;

public:
	bool Load(std::string path);
	const Type* Find(uint id, uint value);
	const DataMap& FindData(uint id) { return mData[id]; }
	const DataMapMap& GetData() { return mData; }

private:
	friend CActivityCopyTestData& CSingleton<CActivityCopyTestData>::Instance();
	CActivityCopyTestData() { }
	~CActivityCopyTestData(){ }

	DataMapMap mData;
};
#define theActivityCopyTestData CSingleton<CActivityCopyTestData>::Instance()

#endif