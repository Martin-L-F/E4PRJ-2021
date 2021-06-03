#include <stdio.h>
#include <iostream>

class Rumble_IF
{
public:
	void startRumble();

	inline void setRumbleTime(unsigned int time)
	{
		_rumble_time = time;
	}

private:
	unsigned int _rumble_time = 200;
};
