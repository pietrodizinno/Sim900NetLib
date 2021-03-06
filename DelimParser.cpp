/* 
* DelimParser.cpp
*
* Created: 2014-06-19 15:45:17
* Author: Tomasz Ścisłowicz
*/


#include "DelimParser.h"

void DelimParser::Init(char *str, uint8_t n, uint8_t strLength)
{
	this->str = str;
	this->strLength = strLength;
	this->tokStart = 0;
	this->n = n;
	parserState = INITIAL;
}

int DelimParser::state_transition(char c, uint8_t state)
{
	switch (state)
	{
		case DELIM:
			if (c == ' ')
				return DELIM;
			if (c == ',')
				return ERR;
		case INITIAL:
			if (c != '"')
				return INSIDE;
			return START_Q;

		case INSIDE_QUOTE:
			if (c == '"')
				return END_Q;
			return INSIDE_QUOTE;
		case INSIDE:
			if (c == ',' || c == ' ')
				return DELIM;
			return INSIDE;
		case START_Q:
			return INSIDE_QUOTE;
		case END_Q:
			if (c == ',' || c == ' ')
				return DELIM;
			return ERR;
	}
	return ERR;
}

bool DelimParser::NextToken()
{
	while (n <= strLength && (parserState != ERR))
	{
		int prevState = parserState;
		parserState = (n == strLength) ? END : state_transition(str[n], parserState);

		if (prevState != parserState)
		{

			//printf("%s -> %s\n", State2Str(prevState), State2Str(parserState));
			// * -> INSIDE
			if (parserState == INSIDE || parserState == INSIDE_QUOTE)
			tokStart = n;

			// INSIDE -> *
			else if (prevState == INSIDE || prevState == INSIDE_QUOTE)
			{
				// token is between tokStart and n
				//while (tokStart < n)
				//	printf("%c", str[tokStart++]);
				//printf("\n");
				n++;
				return true;
			}
		}
		//printf("%c\n", str[n]);

		n++;
	}
	return false;
}

bool DelimParser::NextString(char *d_str, uint8_t length)
{
	if (!NextToken())
		return false;
	int tokLength = n - 1 - tokStart;
	// AS 2 char[2] = ['A',0] tokLength = 2, length = 2
	if (tokLength >= length)
		tokLength = length-1;
	strncpy(d_str, str+tokStart, tokLength);
	d_str[tokLength] = 0;
	return true;
}

bool DelimParser::NextNum(uint16_t &dst, int base /*= 10*/)
{
	if (!NextToken())
		return false;
		dst = 0;

		int x = 1;
		int i = n - 1;
		do
		{
			char c = str[i - 1];
			int digitNum = hexDigitToInt(c);
			if (digitNum == -1)
			return false;

			dst += x*digitNum;
			x *= base;
		}
		while (--i > tokStart);

		return true;
}

int DelimParser::hexDigitToInt(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return -1;
}


/*char *State2Str(int state)
{
switch (state)
{
case INITIAL: return "INITIAL";
case INSIDE: return "INSIDE";
case INSIDE_QUOTE: return "INSIDE_QUOTE";
case START_Q: return "START_Q";
case END_Q: return "END_Q";
case ERR: return "ERR";
case DELIM: return "DELIM";
case END: return "END";
default:
break;
}
}*/