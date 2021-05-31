#include "Pistol_Controller.hpp"

int main() {
    std::cout << "[INFO] Initiating system" << std::endl;
    Pistol_Controller pistol;
    pistol.run();
    std::cin.get();
    return 0;
}