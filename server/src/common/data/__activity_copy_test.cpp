#include "../core/json.h"
#include "../core/util.h"
#include "activity_copy_test.h"

bool CActivityCopyTestData::Load(std::string path)
{
	CJson json;
	if(!json.Read(path + "activity_copy_test.json"))
	{
		return false;
	}

	std::vector<CJson::CJsonLine> &lines = json.GetLines();
	FORLIST(lines, i)
	{
		StringStringMap &pairs = lines[i].pairs;

		Type *data = new Type;
		data->id = CUtil::ToInt(pairs["id"]);
		data->value = CUtil::ToInt(pairs["value"]);
		data->name = pairs["name"];
		data->cycle = CUtil::ToInt(pairs["cycle"]);

		m_data[data->id][data->value] = data;
	}

	return true;
}

const CActivityCopyTestData::Type* CActivityCopyTestData::Find(uint id, uint value)
{
	IF_FIND(m_data, id, iter_map)
	{
		IF_FIND(iter_map->second, value, iter)
		{
			return iter->second;
		}
	}

	return NULL;
}