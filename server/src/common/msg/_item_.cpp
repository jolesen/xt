#include "item.h"

//SItemCell
void SItemCell::Encode(CCoder &coder) const
{
	coder << id;
	coder << count;
}
void SItemCell::Decode(CCoder &coder)
{
	coder >> id;
	coder >> count;
}

//SItem
void SItem::Encode(CCoder &coder) const
{
	//list
	coder << (uint)list.size();
	FORLIST(list, i)
	{
		list[i].Encode(coder);
		if(coder.HasError()) { break; }
	}
}
void SItem::Decode(CCoder &coder)
{
	//list
	list.clear();
	uint len_0 = 0;
	coder >> len_0;
	FOR(len_0, i)
	{
		SItemCell value;
		value.Decode(coder);
		if(coder.HasError()) { break; }
		list.push_back(value);
	}
}
