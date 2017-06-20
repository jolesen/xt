#ifndef _COMMON_MSG_ITEM_H_
#define _COMMON_MSG_ITEM_H_

#include "../core/msg.h"

// SItemCell
struct SItemCell
{
public:
	uint id;
	uint count;

public:
	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

// SItem
struct SItem
{
public:
	std::vector<SItemCell> list;

public:
	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

#endif