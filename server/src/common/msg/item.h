#ifndef _COMMON_MSG_ITEM_H_
#define _COMMON_MSG_ITEM_H_

#include "../core/msg.h"

// SItemCell
struct SItemCell
{
	uint id;
	uint count;

	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

// SItem
struct SItem
{
	std::vector<SItemCell> list;

	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

#endif