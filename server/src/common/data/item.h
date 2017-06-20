#ifndef _COMMON_DATA_ITEM_H_
#define _COMMON_DATA_ITEM_H_

#include "../core/core.h"

// 物品.xls#item
class CItemData
{
public:
	struct Type
	{
		uint id;
		std::string name;
		uint type;
	};
	typedef std::map<uint, Type*> DataMap;

public:
	bool Load(std::string path);
	const Type* Find(uint id);
	const DataMap& GetData() { return m_data; }

private:
	friend CItemData& CSingleton<CItemData>::Instance();
	CItemData() { }
	~CItemData(){ }

	DataMap m_data;
};
#define theItemData CSingleton<CItemData>::Instance()

#endif