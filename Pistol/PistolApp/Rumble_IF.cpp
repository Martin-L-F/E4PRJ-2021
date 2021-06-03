#include "Rumble_IF.hpp"

void Rumble_IF::startRumble() {
  try {
    FILE *fp;
    char msg[32];

    sprintf(msg, "%d", _rumble_time);

    fp = fopen("/dev/RUMBLE", "w");
    fputs(msg, fp);
    fclose(fp);
  } catch (const std::exception &ex) {
    std::cout << "[Rumble_IF][ERROR] Could not write to file. Details: "
              << ex.what() << "." << std::endl;
  }
}
