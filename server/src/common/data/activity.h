#ifndef _COMMON_DATA_ACTIVITY_H_
#define _COMMON_DATA_ACTIVITY_H_

#include "../core/core.h"

// 活动.xls#activity
const uint TEST_ID = 4;
const uint ACTIVITY_TYPE_1 = 1;
const uint ACTIVITY_TYPE_2 = 1;
const uint ACTIVITY_TYPE_3 = 1;
const uint ACTIVITY_TYPE_4 = 1;
const uint ACTIVITY_TYPE_5 = 1;
const uint ACTIVITY_TYPE_6 = 1;
const uint ACTIVITY_TYPE_7 = 1;
const uint ACTIVITY_TYPE_8 = 1;
const uint ACTIVITY_TYPE_9 = 1;
const uint ACTIVITY_TYPE_10 = 1;
const uint ACTIVITY_TYPE_11 = 1;
const uint ACTIVITY_TYPE_12 = 1;
const uint ACTIVITY_TYPE_13 = 1;
const uint ACTIVITY_TYPE_14 = 1;
const uint ACTIVITY_TYPE_15 = 1;
const uint ACTIVITY_TYPE_16 = 1;
const uint ACTIVITY_TYPE_17 = 1;
const uint ACTIVITY_TYPE_18 = 1;
const uint ACTIVITY_TYPE_19 = 1;
const uint ACTIVITY_TYPE_20 = 1;
const uint ACTIVITY_TYPE_21 = 1;
const uint ACTIVITY_TYPE_22 = 1;
const uint ACTIVITY_TYPE_23 = 1;
const uint ACTIVITY_TYPE_24 = 1;
const uint ACTIVITY_TYPE_25 = 1;
const std::string ACT_STR_TYPE_1 = "ty";
const std::string ACT_STR_TYPE_2 = "ty";
const std::string ACT_STR_TYPE_3 = "ty";
const std::string ACT_STR_TYPE_4 = "ty";
const std::string ACT_STR_TYPE_5 = "ty";
const std::string ACT_STR_TYPE_6 = "ty";
const std::string ACT_STR_TYPE_7 = "ty";
const std::string ACT_STR_TYPE_8 = "ty";
const std::string ACT_STR_TYPE_9 = "ty";
const std::string ACT_STR_TYPE_10 = "ty";
const std::string ACT_STR_TYPE_11 = "ty";
const std::string ACT_STR_TYPE_12 = "ty";
const std::string ACT_STR_TYPE_13 = "ty";
const std::string ACT_STR_TYPE_14 = "ty";
const std::string ACT_STR_TYPE_15 = "ty";
const std::string ACT_STR_TYPE_16 = "ty";
const std::string ACT_STR_TYPE_17 = "ty";
const std::string ACT_STR_TYPE_18 = "ty";
const std::string ACT_STR_TYPE_19 = "ty";
const std::string ACT_STR_TYPE_20 = "ty";
const std::string ACT_STR_TYPE_21 = "ty";
const std::string ACT_STR_TYPE_22 = "ty";
const std::string ACT_STR_TYPE_23 = "ty";
const std::string ACT_STR_TYPE_24 = "ty";
const std::string ACT_STR_TYPE_25 = "ty";

class CActivityData
{
public:
	struct Type
	{
		uint id;
		uint ti;
		std::string ts;
		std::string name;
		uint cycle;
	};
	typedef std::map<uint, Type*> DataMap;

public:
	bool Load(std::string path);
	const Type* Find(uint id);
	const DataMap& GetData() { return mData; }

private:
	friend CActivityData& CSingleton<CActivityData>::Instance();
	CActivityData() { }
	~CActivityData(){ }

	DataMap mData;
};
#define theActivityData CSingleton<CActivityData>::Instance()

#endif