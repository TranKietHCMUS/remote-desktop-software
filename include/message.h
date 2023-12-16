#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#define MSG_SIZE 22

#pragma pack(push, 1)

struct msg
{
	int type;
	int flag;
	int data;
	int x, y;
	uint16_t keyCode;
	msg(int type = -1, int flag = 0, int data = 0, int x = 0, int y = 0, uint16_t keyCode = 0)
	{
		this->type = type;
		this->flag = flag;
		this->data = data;
		this->x = x;
		this->y = y;
		this->keyCode = keyCode;
	}
};

#pragma pack(pop)

#endif