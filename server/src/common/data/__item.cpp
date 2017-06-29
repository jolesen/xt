// This file is auto generated.
#include "../core/json.h"
#include "../core/util.h"
#include "item.h"

bool CItemData::Load(std::string path)
{
	CJson json;
	if(!json.Read(path + "item.json"))
	{
		return false;
	}

	std::vector<CJson::CJsonLine> &lines = json.GetLines();
	FORLIST(lines, i)
	{
		StringStringMap &pairs = lines[i].pairs;

		Type *data = new Type;
		data->id = CUtil::ToInt(pairs["id"]);
		data->name = pairs["name"];
		data->type = CUtil::ToInt(pairs["type"]);

		mData[data->id] = data;
	}

	return true;
}

const CItemData::Type* CItemData::Find(uint id)
{
	IF_FIND(mData, id, iter)
	{
		return iter->second;
	}

	return NULL;
}