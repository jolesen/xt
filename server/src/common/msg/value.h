#ifndef _COMMON_MSG_VALUE_H_
#define _COMMON_MSG_VALUE_H_

#include "../core/msg.h"

// SVars
struct SVars
{
public:
	UIntList list;

public:
	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

// QValueVarList
class QValueVarList : public CMsgBase
{
public:
	QValueVarList() { header.type = kMsgTypeClient; header.id = 18101475; }
	~QValueVarList(){ }

protected:
	void EncodeBody(CCoder &coder) const{ }
	void DecodeBody(CCoder &coder){ }
};

// RValueVarList
class RValueVarList : public CMsgBase
{
public:
	RValueVarList() { header.type = kMsgTypeClient; header.id = 17588786; }
	~RValueVarList(){ }

protected:
	void EncodeBody(CCoder &coder) const{ }
	void DecodeBody(CCoder &coder){ }
};

// RValueVarModified
class RValueVarModified : public CMsgBase
{
public:
	RValueVarModified() { header.type = kMsgTypeClient; header.id = 18342553; }
	~RValueVarModified(){ }

public:
	S2UInt var;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// STimes
struct STimes
{
public:
	UIntList list;

public:
	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

// QValueTimeList
class QValueTimeList : public CMsgBase
{
public:
	QValueTimeList() { header.type = kMsgTypeClient; header.id = 18014361; }
	~QValueTimeList(){ }

protected:
	void EncodeBody(CCoder &coder) const{ }
	void DecodeBody(CCoder &coder){ }
};

// RValueTimeList
class RValueTimeList : public CMsgBase
{
public:
	RValueTimeList() { header.type = kMsgTypeClient; header.id = 18283410; }
	~RValueTimeList(){ }

protected:
	void EncodeBody(CCoder &coder) const{ }
	void DecodeBody(CCoder &coder){ }
};

// RValueTimeModified
class RValueTimeModified : public CMsgBase
{
public:
	RValueTimeModified() { header.type = kMsgTypeClient; header.id = 17986452; }
	~RValueTimeModified(){ }

public:
	S2UInt time;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// SDailyResets
struct SDailyResets
{
public:
	UIntList list;

public:
	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

// QValueDailyResetList
class QValueDailyResetList : public CMsgBase
{
public:
	QValueDailyResetList() { header.type = kMsgTypeClient; header.id = 15138912; }
	~QValueDailyResetList(){ }

protected:
	void EncodeBody(CCoder &coder) const{ }
	void DecodeBody(CCoder &coder){ }
};

// RValueDailyResetList
class RValueDailyResetList : public CMsgBase
{
public:
	RValueDailyResetList() { header.type = kMsgTypeClient; header.id = 20044644; }
	~RValueDailyResetList(){ }

protected:
	void EncodeBody(CCoder &coder) const{ }
	void DecodeBody(CCoder &coder){ }
};

// RValueDailyResetModified
class RValueDailyResetModified : public CMsgBase
{
public:
	RValueDailyResetModified() { header.type = kMsgTypeClient; header.id = 19686845; }
	~RValueDailyResetModified(){ }

public:
	S2UInt daily_reset;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

// SCounts
struct SCounts
{
public:
	UIntList list;

public:
	void Encode(CCoder &coder) const;
	void Decode(CCoder &coder);
};

// QValueCountList
class QValueCountList : public CMsgBase
{
public:
	QValueCountList() { header.type = kMsgTypeClient; header.id = 15223480; }
	~QValueCountList(){ }

protected:
	void EncodeBody(CCoder &coder) const{ }
	void DecodeBody(CCoder &coder){ }
};

// RValueCountList
class RValueCountList : public CMsgBase
{
public:
	RValueCountList() { header.type = kMsgTypeClient; header.id = 19225703; }
	~RValueCountList(){ }

protected:
	void EncodeBody(CCoder &coder) const{ }
	void DecodeBody(CCoder &coder){ }
};

// RValueCountModified
class RValueCountModified : public CMsgBase
{
public:
	RValueCountModified() { header.type = kMsgTypeClient; header.id = 15457256; }
	~RValueCountModified(){ }

public:
	S2UInt count;

protected:
	void EncodeBody(CCoder &coder) const;
	void DecodeBody(CCoder &coder);
};

#endif