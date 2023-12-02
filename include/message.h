#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#pragma pack(push, 1)

struct msg
{
	int type;
	int flag;
	int x, y;
	uint16_t keyCode;
};

#pragma pack(pop)

#endif