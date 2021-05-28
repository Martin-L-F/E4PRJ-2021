#pragma once

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>

#include "json.hpp"

using json = nlohmann::json;

/*Client type represents a connected client.*/
class Client {
public:
	// Enum describing types of connection
	enum connectionType {
		dormantBrowser,
		primaryBrowser,
		pistol,
		unknown
	};

	/*Default constructor. Not recommended.*/
	Client() {};

	/*Constructs a client from a shared pointer and a connected time as std::time_t.*/
	Client(std::shared_ptr<websocketpp::connection<websocketpp::config::asio>> ptr, std::time_t connectedTime, json metadata = nullptr) {
		this->ptr = ptr;
		this->connectedTime = connectedTime;
		this->metadata = metadata;
	};

	/*Constructs a client from a shared pointer. Connected time will be set to the creationtime of the object.*/
	Client(std::shared_ptr<websocketpp::connection<websocketpp::config::asio>> ptr, json metadata = nullptr) {
		this->ptr = ptr;
		connectedTime = time(0);
		this->metadata = metadata;
	};

	/*Transmits a message to the client. The string is converted to JSON before being transmitted.
	Returns:
	200 = sucess
	400 = merge error
	503 = client unavalible/disconnected*/
	int send(std::string massage) {
		try
		{
			return send(json::parse(massage));
		}
		catch (const std::exception& ex)
		{
			std::cout << "[WebSocketServer_IF][ERROR] Could not merge replycontent." << std::endl;
			std::cout << "[WebSocketServer_IF][ERROR] " << ex.what() << std::endl;
			return 400;
		}

	}

	/*Transmits a message to the client.
	Returns:
	200 = sucess
	503 = client unavalible/disconnected*/
	int send(json massage) {
		if (ptr == nullptr) return 503;
		if (ptr->get_state() != websocketpp::session::state::open) return 503;
		std::cout << "[WebSocketServer_IF][INFO] Transmitting message. Content: '" << massage.dump() << "'. Client: '" << ptr->get_remote_endpoint() << "'." << std::endl;
		ptr->send(massage.dump());
		return 200;
	}

	/*Returns the state of the client.
	Returns:
	0 = connecting
	1 = open
	2 = closing
	3 = closed
	503 = client unavalible*/
	int getState() {
		if (ptr == nullptr) return 503;
		return ptr->get_state();
	}

	/*Returns the type of the connection.*/
	connectionType getType() {
		return type;
	}

	/*Sets the type of the connection.*/
	void setType(connectionType type) {
		this->type = type;
	}

	/*Returns the time of connection as std::time_t.*/
	std::time_t getConnectionTime() {
		return connectedTime;
	}

	/*Returns the client metadata json-object.*/
	json getMetadata() {
		return metadata;
	}

	/*Merges new metadata with existing metadata.
	Returns:
	200 = sucess
	500 = error*/
	int mergeMetadata(json metadata) {
		try
		{
			this->metadata.merge_patch(metadata);
			return 200;
		}
		catch (const std::exception& ex)
		{
			std::cout << "[WebSocketServer_IF][ERROR] Could not merge metadata for client." << std::endl;
			std::cout << "[WebSocketServer_IF][ERROR] " << ex.what() << std::endl;
			return 500;
		}
	}

	/*Returns wether or not the shared pointer belongs to this client.*/
	bool is(std::shared_ptr<websocketpp::connection<websocketpp::config::asio>> ptr) {
		return this->ptr == ptr;
	}

private:
	std::shared_ptr<websocketpp::connection<websocketpp::config::asio>> ptr = nullptr;
	std::time_t connectedTime = NULL;
	connectionType type = connectionType::unknown;
	json metadata;
};