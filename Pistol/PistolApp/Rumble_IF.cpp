#include "Rumble_IF.hpp"

void Rumble_IF::startRumble()
{
	try
	{
		FILE *fp;

		fp = fopen("/dev/RUMBLE", "w");
		fputc('1', fp);
		fclose(fp);
	}
	catch (const std::exception &ex)
	{
		std::cout << "[Rumble_IF][ERROR] Could not write to file. Details: " << ex.what() << "." << std::endl;
	}
}
