#include <stdio.h>
#include "Rumble_IF.hpp"

Rumble_IF::Rumble_IF() {};
Rumble_IF::~Rumble_IF() {};

void Rumble_IF::startRumble()
{
	FILE* fp;

	fp = fopen(_devPath, "w");
	fputc('1', fp);
	fclose(fp);
}
