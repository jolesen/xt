// This file is auto generated.
#include "../core/json.h"
#include "../core/util.h"
#include "list_test.h"

bool CListTestData::Load(std::string path)
{
	CJson json;
	if(!json.Read(path + "list_test.json"))
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
		data->cycle = CUtil::ToInt(pairs["cycle"]);

		StringList values;
		values = CUtil::SplitString(pairs["ints"], ",");
		FORLIST(values, j)
		{
			uint val = CUtil::ToInt(values[j]);
			if(val != 0)
			{
				data->ints.push_back(val);
			}
		}

		values = CUtil::SplitString(pairs["s2s"], ",");
		FORLIST(values, j)
		{
			S3UInt val;
			if(sscanf(values[j].c_str(), "%u%%%u%%%u", &val.type, &val.id, &val.value) == 3)
			{
				data->s2s.push_back(val);
			}
		}

		values = CUtil::SplitString(pairs["s3s"], ",");
		FORLIST(values, j)
		{
			S3UInt val;
			if(sscanf(values[j].c_str(), "%u%%%u%%%u", &val.type, &val.id, &val.value) == 3)
			{
				data->s3s.push_back(val);
			}
		}

		values = CUtil::SplitString(pairs["strs"], ",");
		FORLIST(values, j)
		{
			std::string val = values[j];
			if(val != "")
			{
				data->strs.push_back(val);
			}
		}
		data->od = CUtil::ToInt(pairs["od"]);

		mData[data->id] = data;
	}

	return true;
}

const CListTestData::Type* CListTestData::Find(uint id)
{
	IF_FIND(mData, id, iter)
	{
		return iter->second;
	}

	return NULL;
}