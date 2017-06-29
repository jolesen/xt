#include "../core/json.h"
#include "../core/util.h"
#include "activity.h"

bool CActivityData::Load(std::string path)
{
	CJson json;
	if(!json.Read(path + "activity.json"))
	{
		return false;
	}

	std::vector<CJson::CJsonLine> &lines = json.GetLines();
	FORLIST(lines, i)
	{
		StringStringMap &pairs = lines[i].pairs;

		Type *data = new Type;
		data->id = CUtil::ToInt(pairs["id"]);
		data->ti = CUtil::ToInt(pairs["ti"]);
		data->ts = pairs["ts"];
		data->name = pairs["name"];
		data->cycle = CUtil::ToInt(pairs["cycle"]);

		mData[data->id] = data;
	}

	return true;
}

const CActivityData::Type* CActivityData::Find(uint id)
{
	IF_FIND(mData, id, iter)
	{
		return iter->second;
	}

	return NULL;
}