#include "Speaker_IF.h"

void Speaker_IF::playSound(string sound)
{
	string playSound = "omxplayer -local --no-keys --vol 500 " + sound + ".mp3 &";
	system(playSound.c_str());
}
