#include "test.h"

//QTestTime
void QTestTime::EncodeBody(CCoder &coder) const
{
	coder << time;
	coder << seq;
	coder << s;
	coder << tst;
	coder << imap;
	coder << smap;
}
void QTestTime::DecodeBody(CCoder &coder)
{
	coder >> time;
	coder >> seq;
	coder >> s;
	coder >> tst;
	coder >> imap;
	coder >> smap;
}

//RTestTime
void RTestTime::EncodeBody(CCoder &coder) const
{
	coder << time;
	coder << seq;
}
void RTestTime::DecodeBody(CCoder &coder)
{
	coder >> time;
	coder >> seq;
}
