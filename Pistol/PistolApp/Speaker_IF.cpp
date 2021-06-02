#include "Speaker_IF.h"

void Speaker_IF::playSound(string sound)
{
	try
	{
		string playSound = "omxplayer -local --no-keys --vol 500 " + sound + ".mp3 &";
		system(playSound.c_str());
	}
	catch (const std::exception &ex)
	{
		std::cout << "[Speaker_IF][ERROR] Failed to play file." << std::endl;
	}
}
