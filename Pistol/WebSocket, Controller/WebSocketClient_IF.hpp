#pragma once
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>
#include <set>

#include "json.hpp"

using json = nlohmann::json;


// WebSocket client to connecto to TargetBeam ControlUnit.
class WebSocketClient_IF {
public:
	WebSocketClient_IF() {
		// Find config file
		readConfigfile();
	}

	~WebSocketClient_IF() {
		try
		{
			stopFlag = true;
			if (client != nullptr) {
				client->stop();
			}
			if (runtimeThread.joinable()) {
				runtimeThread.join();
			}
			if (broadcastThread.joinable()) {
				broadcastThread.join();
			}
		}
		catch (const std::exception& ex)
		{
			std::cout << "[WebSocketClient_IF][ERROR] Failed to stop the client on destruction. Details: " << ex.what() << std::endl;
		}
	}

	// Starts the client. Async.
	void run() {
		std::cout << "[WebSocketClient_IF][INFO] Starting client." << std::endl;
		runtimeThread = std::thread(std::bind(&WebSocketClient_IF::_run, this));
	}

	/*
		Set a handler for incomming messages.
		This callback will only be triggered by valid messages where the conent is JSON-passable.
		WebSocketClient_IF automaticly takes care of callback messages so theise will not trigger this callback.
	*/
	void set_onMessage_handler(std::function<void(json)> cb) {
		onMessage = cb;
	}

	/*
		Set a handler for when the connection succeeds.
	*/
	void set_onConnected_handler(std::function<void()> cb) {
		onConnected = cb;
	}

	/*
		Set a handler for when the client disconnects.
	*/
	void set_onDisconnected_handler(std::function<void(int)> cb) {
		onDisconnected = cb;
	}

	/*
		Set a datagathering handler to let WebSocketClient_IF get the batterylevel when it is needed.
	*/
	void set_getBattery_function(std::function<int()> cb) {
		getBattery = cb;
	}

	/*
		Set a datagathering handler to let WebSocketClient_IF get the chargingstate when it is needed.
	*/
	void set_getCharging_function(std::function<bool()> cb) {
		getCharging = cb;
	}

	/*
		Transmits message to server if connected, otherwise saves the message to be transmitted later.
		This overload requires a callbackfunction to be executed upon received answer.
	*/
	void send(json message, std::function<void(json)> cb, bool discardOnError = false) {

		// Checks if the list is full.
		if (callbacklist.size() >= 10000) {
			std::cout << "[WebSocketClient_IF][ERROR] The callbacklist is full. Message will not be transmitted." << std::endl;
			return;
		}

		json _message = message;
		long callbackid = 0;

		// Generate a random callback ID that is not on the list.
		do {
			callbackid = rand() % 10000 + 1;
		} while (callbacklist.find(callbackid) != callbacklist.end());

		// Add the callback to the list.
		callbacklist[callbackid] = cb;

		// Add the ID to the message.
		message["callback"] = callbackid;

		// Use the overload of this function to transmit the message.
		send(message, discardOnError);
	}

	// Transmits message to server if connected, otherwise saves the message to be transmitted later.
	void send(json message, bool discardOnError = false) {
		// Checks if the client is disconnected.
		if (!connected || client == nullptr) {
			// If the message is set to discardOnError, then delete it from the callbacklist, if is there.
			if (discardOnError) {
				if (!message["callback"].is_null()) {
					callbacklist.erase(message["callback"]);
				}
				return;
			}

			// If the message should not be discareded, save it.
			std::cout << "[WebSocketClient_IF][WARN] Storing message for later transmission. Content: '" << message.dump() << "'." << std::endl;
			messagePendingQueue.push(message);
			return;
		}

		// If the client is connected, send the message.
		std::cout << "[WebSocketClient_IF][INFO] Transmitting message. Content: '" << message.dump() << "'." << std::endl;
		websocketpp::lib::error_code ec;
		client->send(m_hdl, message.dump(), websocketpp::frame::opcode::text, ec);

		// Error print.
		if (ec) {
			std::cout << "[WebSocketClient_IF][ERROR] Failed to transmit message. Detail: " << ec.message() << "." << std::endl;
		}

	}

private:
	// WebSocket client.
	websocketpp::client<websocketpp::config::asio_client>* client = nullptr;

	// Connectionhandle to WebSocket connection.
	websocketpp::connection_hdl m_hdl;

	// Thread to run void _run().
	std::thread runtimeThread;

	int retryDelay = 10000;

	// Connection uri to WebSocket Server.
	std::string uri = "ws://localhost:3000"; //"ws://targetbeam.com:3000";

	// Callback for incomming message.
	std::function<void(json)> onMessage = nullptr;
	// Callback for client connected.
	std::function<void()> onConnected = nullptr;
	// Callback for client disconnected.
	std::function<void(int)> onDisconnected = nullptr;

	// Callbacklist contains id and function for pending callbacks.
	std::map<long, std::function<void(json)>> callbacklist;

	// Queue to store pending message when the server is disconnected.
	std::queue<json> messagePendingQueue;

	// Indicating wether or not the server is connected.
	bool connected = false;

	// Callbackfunction to get batterylevel
	std::function<int()> getBattery = nullptr;

	// Callbackfunction to get charging condition
	std::function<bool()> getCharging = nullptr;

	// Indication to the run-loop that it should not try again.
	bool stopFlag = false;


	// Function to run in std::thread runtimeThread. Starts and runs the client.
	void _run() {
		while (!stopFlag) {
			// Create WebSocket objects.
			websocketpp::client<websocketpp::config::asio_client> _client;
			websocketpp::client<websocketpp::config::asio_client>::connection_ptr _con;

			// Set output level
			_client.set_access_channels(websocketpp::log::alevel::none);
			_client.set_error_channels(websocketpp::log::elevel::none);

			// Initialize ASIO
			_client.init_asio();

			// Connect handlers
			_client.set_message_handler(std::bind(&WebSocketClient_IF::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
			_client.set_open_handler(std::bind(&WebSocketClient_IF::on_connect, this, websocketpp::lib::placeholders::_1));
			_client.set_close_handler(std::bind(&WebSocketClient_IF::on_disconnect, this, websocketpp::lib::placeholders::_1));
			_client.set_fail_handler(std::bind(&WebSocketClient_IF::on_error, this, websocketpp::lib::placeholders::_1));

			// Set the client pointer.
			client = &_client;

			websocketpp::lib::error_code errorCode;

			// Get connectionpointer.
			_con = _client.get_connection(uri, errorCode);
			if (errorCode) {
				std::cout << "[WebSocketClient_IF][ERROR] Unable to get connection pointer to URI. Code: " << errorCode.value() << std::endl;
				return;
			}

			// Set handle.
			m_hdl = _con->get_handle();

			// Connect to the server.
			_client.connect(_con);

			// Run the client. Blocking call until error.
			_client.run();

			// On an error, reset the variables.
			connected = false;
			client = nullptr;

			// Wait before retry.
			std::this_thread::sleep_for(std::chrono::milliseconds(retryDelay));
		}
	}

	// Handler for connection established.
	void on_connect(websocketpp::connection_hdl hdl) {
		connected = true;
		std::cout << "[WebSocketClient_IF][INFO] Connected." << std::endl;
		send(getRequestVerificationMessage(), std::bind(&WebSocketClient_IF::verified, this, std::placeholders::_1), true);
	}

	// Function to be used as a callback on the verification message.
	void verified(json reply) {
		if (reply["status"] != 200) {
			std::cout << "[WebSocketClient_IF][ERROR] Verification failed. Details: '" << reply.dump() << "'" << std::endl;
			return;
		}
		std::cout << "[WebSocketClient_IF][INFO] Verified." << std::endl;
		int pendingSize = messagePendingQueue.size();
		if (pendingSize > 0) {
			std::cout << "[WebSocketClient_IF][INFO] Atempting to transmit " << pendingSize << " old messages." << std::endl;
		}
		for (int i = 0; i < pendingSize; i++) {
			json message = messagePendingQueue.front();
			send(message, true);
			messagePendingQueue.pop();
		}

		startBroadcastLoop();

		if (onConnected != nullptr) {
			onConnected();
		}
	}

	// Handler for connection disconnected.
	void on_disconnect(websocketpp::connection_hdl hdl) {
		connected = false;
		std::cout << "[WebSocketClient_IF][ERROR] Disconnected." << std::endl;
		if (onDisconnected != nullptr) {
			onDisconnected(500);
		}
		stopBroadcastLoop();
	}

	// Handler for connection error.
	void on_error(websocketpp::connection_hdl hdl) {
		connected = false;
		stopBroadcastLoop();
	}

	// Handler for incomming message.
	void on_message(websocketpp::connection_hdl hdl, websocketpp::config::asio_tls_client::message_type::ptr message) {

		// Retrieve the message as a string.
		std::string rawContent = message->get_payload();
		std::cout << "[WebSocketClient_IF][INFO] Message recieved. Content: '" << rawContent << "'." << std::endl;

		// Pass the message to json in a try-state.
		json content;
		try
		{
			content = json::parse(rawContent);
		}
		catch (const std::exception& ex)
		{
			std::cout << "[WebSocketClient_IF][ERROR] Failed to pass incomming message to json. Terminating handle. Details: " << ex.what() << std::endl;
			return;
		}

		// Check if the message has a type.
		if (!content["type"].is_string()) {
			std::cout << "[WebSocketClient_IF][ERROR] Invalid data in incomming message. Terminating handle." << std::endl;
			return;
		}

		// Handle the message internaly if it is a callback-answer.
		if (content["type"] == "Callback") {
			std::cout << "[WebSocketClient_IF][INFO] Message is a callback." << std::endl;

			// This client only sets numeric callback IDs so there is a check to see if the incomming ID is also numeric.
			if (content["callback"].is_number()) {

				// Checks if the callback is in the callbacklist.
				if (callbacklist.find(content["callback"]) != callbacklist.end()) {
					try
					{
						// Tries to call the function. This is in a try-state since many errors can occur when calling and the try-state catches any errors that gets thrown when executing the callbacks functionality.
						callbacklist[content["callback"]](content);
						callbacklist.erase(content["callback"]);
						return;
					}
					catch (const std::exception& ex)
					{
						std::cout << "[WebSocketClient_IF][ERROR] An error occurred then calling calback function for message. Terminating handle. Detail: " << ex.what() << std::endl;
						return;
					}
				}
				else {
					std::cout << "[WebSocketClient_IF][ERROR] Callback ID could not be found in callback list. Terminating handle." << std::endl;
					return;
				}
			}
			else {
				std::cout << "[WebSocketClient_IF][ERROR] Error in callback ID. Terminating handle." << std::endl;
				return;
			}
		}

		// Call the callback if is has been set.
		if (onMessage != nullptr) {
			onMessage(content);
		}
	}

	// Thread to run broadcastLoop().
	std::thread broadcastThread;

	long broadcastDelay = 3000;

	// Function to run in std::thread broadcastThread.
	void broadcastLoop() {
		while (connected) {
			send(getBatteryUpdateMessage(), true);
			std::this_thread::sleep_for(std::chrono::milliseconds(broadcastDelay));
		}
	}

	// Starts broadcast loop.
	void startBroadcastLoop() {
		broadcastThread = std::thread(std::bind(&WebSocketClient_IF::broadcastLoop, this));
	}

	// Stops broadcast loop.
	void stopBroadcastLoop() {
		if (broadcastThread.joinable()) {
			broadcastThread.join();
		}
	}

	// Returns a full verification request by calling getBattery() and getCharging() to fill data.
	json getRequestVerificationMessage() {
		json message;
		message["type"] = "RequestConnectionVerification";
		message["connectionType"] = "pistol";
		message["battery"] = 0;
		if (getBattery != nullptr) {
			message["battery"] = getBattery();
		}
		message["charging"] = false;
		if (getCharging != nullptr) {
			message["charging"] = getCharging();
		}
		return message;
	}

	// Returns a full systemupdate message by calling getBattery() and getCharging() to fill data.
	json getBatteryUpdateMessage() {
		json message;
		message["type"] = "BatteryUpdate";
		message["battery"] = 0;
		if (getBattery != nullptr) {
			message["battery"] = getBattery();
		}
		message["charging"] = false;
		if (getCharging != nullptr) {
			message["charging"] = getCharging();
		}
		return message;
	}

	void readConfigfile() {
		try {
			std::ifstream configStream("config.json");
			json config;
			if (configStream.is_open()) {
				std::cout << "[WebSocketClient_IF][INFO] Configurationfile found." << std::endl;
				configStream >> config;
				configStream.close();

				if (config.is_object())
				{
					// Get broadcastDelay
					if (config["WebSocketClient_IF"]["broadcastDelay"].is_number())
					{
						broadcastDelay = config["WebSocketClient_IF"]["broadcastDelay"];
						std::cout << "[WebSocketClient_IF][INFO] BroadcastDelay changed to '" << broadcastDelay << "'." << std::endl;
					}
					else
					{
						std::cout << "[WebSocketClient_IF][WARN] Syntaxerror in configurationfile." << std::endl;
					}

					// Get retryDelay
					if (config["WebSocketClient_IF"]["retryDelay"].is_number())
					{
						retryDelay = config["WebSocketClient_IF"]["retryDelay"];
						std::cout << "[WebSocketClient_IF][INFO] RetryDelay changed to '" << retryDelay << "'." << std::endl;
					}
					else
					{
						std::cout << "[WebSocketClient_IF][WARN] Syntaxerror in configurationfile." << std::endl;
					}

					// Get uri
					if (config["WebSocketClient_IF"]["uri"].is_string())
					{
						uri = config["WebSocketClient_IF"]["uri"];
						std::cout << "[WebSocketClient_IF][INFO] Uri changed to '" << uri << "'." << std::endl;
					}
					else
					{
						std::cout << "[WebSocketClient_IF][WARN] Syntaxerror in configurationfile" << std::endl;
					}


					// Add more config here.


					std::cout << "[WebSocketClient_IF][INFO] End of configurationfile." << std::endl;
				}
				else
				{
					std::cout << "[WebSocketClient_IF][INFO] Configurationfile is empty." << std::endl;
				}
			}
			else
			{
				std::cout << "[WebSocketClient_IF][INFO] Configurationfile was not found." << std::endl;
			}
		}
		catch (const std::exception& ex)
		{
			std::cout << "[WebSocketClient_IF][WARN] An error occurred while reading configurationfile." << std::endl;
			std::cout << "[WebSocketClient_IF][WARN] " << ex.what() << std::endl;
		}
	}
};