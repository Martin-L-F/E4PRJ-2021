#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>

#include "WebSocketClient_IF.hpp"

class Pistol_Controller {
public:
	Pistol_Controller() {
		client.set_onMessage_handler(std::bind(&Pistol_Controller::onMessage, this, std::placeholders::_1));
		client.set_getBattery_function(std::bind(&Pistol_Controller::getBatteryLevel, this));
	}

	void run() {
		std::cout << std::endl << " ============================ " << std::endl
			<< " === TargetBeam PistolApp === " << std::endl
			<< " ============================ " << std::endl << std::endl;
		client.run();
	}
private:
	WebSocketClient_IF client;


	void onMessage(json message) {
		if (message["type"] == "armPistol") {

			if (!message["difficulty"].is_string() || !message["frequency"]["range"].is_number() || !message["frequency"]["index"].is_number()) {
				std::cout << "[Pistol_Controller][WARN] Content error in message of type: 'armPistol'." << std::endl;
				return;
			}


			// Handle this.

		}


		else if (message["type"] == "preventStandby") {

			// Handle this.
		}


		else if (message["type"] == "disarmPistol") {

			// Handle this.
		}


		else if (message["type"] == "preventStandby") {

			// Handle this.
		}


		else if (message["type"] == "preventStandby") {

			// Handle this.
		}


		else
		{
			std::cout << "[Pistol_Controller][WARN] Unresolved message type" << std::endl;
		}
	}

	int getBatteryLevel() {
		return rand() % 100 + 1;
	}
};