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
	const DataMap& FindData(uint id) { return m_data[id]; }
	const DataMapMap& GetData() { return m_data; }

private:
	friend CActivityCopyTestData& CSingleton<CActivityCopyTestData>::Instance();
	CActivityCopyTestData() { }
	~CActivityCopyTestData(){ }

	DataMapMap m_data;
};
#define theActivityCopyTestData CSingleton<CActivityCopyTestData>::Instance()

#endif