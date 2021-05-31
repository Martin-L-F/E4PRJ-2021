#include "ControlUnit_Controller.hpp"

int main() {
	std::cout << "[INFO] Initiating system" << std::endl;
	ControlUnit_Controller ControlUnit;
	ControlUnit.run();
	std::cin.get();
	return 0;
}