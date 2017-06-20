#ifndef _COMMON_MSG_TEST_H_
#define _COMMON_MSG_TEST_H_

#include "../core/msg.h"

// QTestTime
class QTestTime : public CMsgBase
{
public:
	QTestTime() { header.type = kMsgTypeClient; header.id = 15795049; }
	~QTestTime(){ }

public:
	uint time;
	uint seq;
	std::string s;
	std::string tst;
	UIntStringMap imap;
	StringStringMap smap;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// RTestTime
class RTestTime : public CMsgBase
{
public:
	RTestTime() { header.type = kMsgTypeClient; header.id = 15533661; }
	~RTestTime(){ }

public:
	uint time;
	uint seq;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

#endif