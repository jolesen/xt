#include "value.h"

//SVars
void SVars::Encode(CCoder &coder) const
{
	coder << list;
}
void SVars::Decode(CCoder &coder)
{
	coder >> list;
}

//RValueVarModified
void RValueVarModified::EncodeBody(CCoder &coder) const
{
	coder << var;
}
void RValueVarModified::DecodeBody(CCoder &coder)
{
	coder >> var;
}

//STimes
void STimes::Encode(CCoder &coder) const
{
	coder << list;
}
void STimes::Decode(CCoder &coder)
{
	coder >> list;
}

//RValueTimeModified
void RValueTimeModified::EncodeBody(CCoder &coder) const
{
	coder << time;
}
void RValueTimeModified::DecodeBody(CCoder &coder)
{
	coder >> time;
}

//SDailyResets
void SDailyResets::Encode(CCoder &coder) const
{
	coder << list;
}
void SDailyResets::Decode(CCoder &coder)
{
	coder >> list;
}

//RValueDailyResetModified
void RValueDailyResetModified::EncodeBody(CCoder &coder) const
{
	coder << dailyReset;
}
void RValueDailyResetModified::DecodeBody(CCoder &coder)
{
	coder >> dailyReset;
}

//SCounts
void SCounts::Encode(CCoder &coder) const
{
	coder << list;
}
void SCounts::Decode(CCoder &coder)
{
	coder >> list;
}

//RValueCountModified
void RValueCountModified::EncodeBody(CCoder &coder) const
{
	coder << count;
}
void RValueCountModified::DecodeBody(CCoder &coder)
{
	coder >> count;
}
