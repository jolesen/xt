#ifndef _COMMON_DATA_LIST_TEST_H_
#define _COMMON_DATA_LIST_TEST_H_

#include "../core/core.h"

// 活动.xls#list_test
class CListTestData
{
public:
	struct Type
	{
		uint id;
		std::string name;
		uint cycle;
		UIntList ints;
		S3UIntList s2s;
		S3UIntList s3s;
		StringList strs;
		uint od;
	};
	typedef std::map<uint, Type*> DataMap;

public:
	bool Load(std::string path);
	const Type* Find(uint id);
	const DataMap& GetData() { return mData; }

private:
	friend CListTestData& CSingleton<CListTestData>::Instance();
	CListTestData() { }
	~CListTestData(){ }

	DataMap mData;
};
#define theListTestData CSingleton<CListTestData>::Instance()

#endif