#pragma once
#define ASIO_STANDALONE

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <functional>
#include <chrono>
#include <thread>

#include "json.hpp"
#include "Message.hpp"

using json = nlohmann::json;

typedef websocketpp::server<websocketpp::config::asio> server;

class WebSocketServer_IF {
public:
	WebSocketServer_IF() {
		// Find config file
		readConfigfile();

		// Set logging settings for websocketpp
		m_endpoint.set_error_channels(websocketpp::log::elevel::none);
		m_endpoint.set_access_channels(websocketpp::log::alevel::none);

		// Initialize Asio
		m_endpoint.init_asio();

		// Set message handler
		m_endpoint.set_message_handler(websocketpp::lib::bind(&WebSocketServer_IF::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

		// Set open handler
		m_endpoint.set_open_handler(bind(&WebSocketServer_IF::on_open, this, websocketpp::lib::placeholders::_1));
		// Set close handler
		m_endpoint.set_close_handler(bind(&WebSocketServer_IF::on_close, this, websocketpp::lib::placeholders::_1));

		//Set HTTP handler
		m_endpoint.set_http_handler(bind(&WebSocketServer_IF::on_http, this, websocketpp::lib::placeholders::_1));
	}

	~WebSocketServer_IF() {
		std::cout << "[WebSocketServer_IF][INFO] Stopping server." << std::endl;
		m_endpoint.stop();
		endpointThread.join();
		std::cout << "[WebSocketServer_IF][INFO] Server stopped." << std::endl;
	}

	/*
		Register a function to be called when a message is recieved.
		Callback function should be of type void(Message*).
	*/
	void add_onMessage_Handler(std::function<void(Message*)> cb) {
		onMessage = cb;
	}

	/*
		Register a function to be called when a client connects.
		Callback function should be of type void(Client*).
	*/
	void add_onConnected_Handler(std::function<void(Client*)> cb) {
		onConnected = cb;
	}

	/*
		Register a function to be called when a client disconnects.
		Callback function should be of type void(Client*).
	*/
	void add_onDisconnected_Handler(std::function<void(Client*)> cb) {
		onDisconnected = cb;
	}

	// Starts the server. Async.
	void run() {
		endpointThread = std::thread(&WebSocketServer_IF::_run, this);
	}

	// Returns a pointer to a vector of all connected clients.
	std::vector<Client>* getClients() {
		return &connections;
	}

	/*
		Returns a vector of Client pointes for all clients of a certan type.
		Returns an empty vector if no clients of the given type is connected.
	*/
	std::vector<Client*> getConnectionsOfType(Client::connectionType type) {
		std::vector<Client*> res;
		for (auto& con : connections) {
			if (con.getType() == type) {
				res.push_back(&con);
			}
		}
		return res;
	}

	// Changes the connectiontype of a connection and transmits a message if needed
	void redefineConnection(Client* con, Client::connectionType newType, bool broadcastUpdate = true) {
		if (con->getType() != newType) {
			con->setType(newType);

			json informMessage;
			json meta;
			switch (newType)
			{
			case Client::connectionType::dormantBrowser:
				informMessage["type"] = "SetToDormantBrowser";
				break;
			case Client::connectionType::primaryBrowser:
				informMessage["type"] = "UpgradeToPrimaryBrowser";
				break;
			case Client::connectionType::pistol:
				informMessage["type"] = "UpgradeToPistol";
				meta["battery"] = 0;
				meta["charging"] = false;
				con->mergeMetadata(meta);
				break;
			case Client::connectionType::unknown:
				informMessage["type"] = "DowngradeToUnknown";
				meta["battery"] = nullptr;
				meta["charging"] = nullptr;
				con->mergeMetadata(meta);
				break;
			default:
				std::cout << "[WebSocketServer_IF][WARN] Tried to upgade to an unknown connectiontype." << std::endl;
				break;
			}

			if (!informMessage["type"].is_null()) {
				con->send(informMessage);
			}
			if (broadcastUpdate && (newType == Client::connectionType::pistol || newType == Client::connectionType::primaryBrowser)) {
				broadcastConnections();
			}
		}
	}

	// Sends a list of the connected pistols to the primary browser
	void broadcastConnections() {
		auto res = getConnectionsOfType(Client::connectionType::primaryBrowser);
		if (res.size() > 0) {
			json message;

			message["type"] = "Pistollist";
			message["pistols"] = json::array();
			for (auto& con : getConnectionsOfType(Client::connectionType::pistol)) {
				message["pistols"].push_back(con->getMetadata());
			}
			res[0]->send(message);
		}
		else {
			std::cout << "[WebSocketServer_IF][WARN] Canceled broadcast of information. No primary browser was found" << std::endl;
		}
	}

private:
	// On message callback funton.
	std::function<void(Message*)> onMessage = nullptr;

	// On new connection callback function.
	std::function<void(Client*)> onConnected = nullptr;

	// On connection ended callback function.
	std::function<void(Client*)> onDisconnected = nullptr;


	// The actual server
	server m_endpoint;

	// Thead running the endpoint
	std::thread endpointThread;

	// Port
	int port = 3000;

	// Document root of HTTP requests
	std::string httpfilelocation = "httpcontent/";

	// Vector containing pointers to all connections
	std::vector<Client> connections;

	// Server start. Runs in thread

	void _run() {
		// Retrying to allocate the port if it fails
		bool portSucess = false;
		while (portSucess == false)
		{
			try
			{
				// Listen on port 3000
				m_endpoint.listen(asio::ip::tcp::v4(), port);
				portSucess = true;
			}
			catch (const std::exception& ex)
			{
				std::cout << "[WebSocketServer_IF][ERROR] Failed to allocate port 3000. Retry will commence in 10 seconds" << std::endl;
				std::cout << "[WebSocketServer_IF][ERROR] " << ex.what() << std::endl;
				std::this_thread::sleep_for((std::chrono::milliseconds)10000);
			}
		}

		// Queues a connection accept operation
		m_endpoint.start_accept();

		std::cout << "[WebSocketServer_IF][INFO] Starting server on port " << port << "." << std::endl
			<< std::endl;

		// Start the Asio io_service run loop
		m_endpoint.run();
	}

	// Message handler
	void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {

		try
		{
			// Connection pointer
			std::shared_ptr<websocketpp::connection<websocketpp::config::asio>> connectionPointer = m_endpoint.get_con_from_hdl(hdl);

			std::cout << std::endl
				<< "[WebSocketServer_IF][INFO] Message recieved from: " << connectionPointer->get_remote_endpoint() << " | saying: " << msg->get_payload() << std::endl;

			// Find connection from connectionlist
			Client* con = nullptr;
			for (auto& _con : connections)
			{
				if (_con.is(connectionPointer))
				{
					con = &_con;
				}
			}
			if (con == nullptr)
			{
				std::cout << "[WebSocketServer_IF][ERROR] Connection was not found in connectionlist. Terminating handle" << std::endl;
				return;
			}

			// Input as JSON
			json input;
			try
			{
				input = json::parse(msg->get_payload());
			}
			catch (const std::exception& ex)
			{
				std::cout << "[WebSocketServer_IF][ERROR] Could not pass incomming message as JSON. Terminating handle. Errordetails: " << ex.what() << std::endl;
				return;
			}

			Message message(input, con);
			if (onMessage != nullptr) {
				try
				{
					onMessage(&message);
				}
				catch (const std::exception& ex)
				{
					std::cout << "[WebSocketServer_IF][ERROR] Fatal error in message handle callback. Terminating handle. Errordetails: " << ex.what() << std::endl;
				}

			}
			else {
				std::cout << "[WebSocketServer_IF][WARN] No handler for event 'onMessage' found." << std::endl;
				message.addToResponse("{\"status\": 501}"_json);
			}

			if (message.is_requestingReply()) {
				if (!message.is_replyUpdated()) {
					message.addToResponse("{\"status\": 204}"_json);
				}
				message.getClient()->send(message.getResponseContent());
			}
		}
		catch (const std::exception& ex)
		{
			std::cout << "[WebSocketServer_IF][ERROR] Fatal error on incomming message. Terminating handle. Errordetails: " << ex.what() << std::endl;
		}
	}

	// Open handler
	void on_open(websocketpp::connection_hdl hdl) {
		// Translating connection-handle to connection-pointer which is much more functional
		std::shared_ptr<websocketpp::connection<websocketpp::config::asio>> connectionPointer = m_endpoint.get_con_from_hdl(hdl);

		// Console message
		std::cout << "[WebSocketServer_IF][INFO] Connection established with: " << (connectionPointer)->get_remote_endpoint() << std::endl;

		// Adds the pointer to a vector for later use
		json metadata;
		metadata["ID"] = connectionPointer->get_remote_endpoint().substr(connectionPointer->get_remote_endpoint().find_last_of(":"));
		connections.push_back({ connectionPointer, time(0), metadata });

		if (onConnected != nullptr) {
			onConnected(&connections[connections.size() - 1]);
		}
		else {
			std::cout << "[WebSocketServer_IF][WARN] No handler for event 'onConnected' found." << std::endl;
		}
	}

	// Close handler
	void on_close(websocketpp::connection_hdl hdl) {
		// Connectionpointer for disconnected connection
		std::shared_ptr<websocketpp::connection<websocketpp::config::asio>> pointer = m_endpoint.get_con_from_hdl(hdl);

		// Removes pointer from vector
		for (std::vector<Client>::iterator it = connections.begin(); it < connections.end(); it++)
		{
			if ((*it).is(pointer))
			{
				if (onDisconnected != nullptr) {
					onDisconnected(&(*it));
				}
				else {
					std::cout << "[WebSocketServer_IF][WARN] No handler for event 'onConnected' found." << std::endl;
				}

				if ((*it).getType() == Client::connectionType::primaryBrowser) {
					std::cout << "[WebSocketServer_IF][INFO] Primary browser disconnected" << std::endl;
					auto res = getConnectionsOfType(Client::connectionType::dormantBrowser);
					if (res.size() > 0) {
						redefineConnection(res[0], Client::connectionType::primaryBrowser);
					}
					else {
						std::cout << "[WebSocketServer_IF][WARN] No new primary browser was found" << std::endl;
					}
				}
				connections.erase(it);
				break;
			}
		}

		broadcastConnections();
	}

	// Handler for HTTP requests
	void on_http(websocketpp::connection_hdl hdl) {
		// Upgrade our connection handle to a full connection_ptr
		server::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);

		std::ifstream file;
		std::string filename = con->get_uri()->get_resource();
		std::string response;
		std::string filePath = filename;

		if (filename == "/") {
			filePath = httpfilelocation + "index.html";
		}
		else {
			filePath = httpfilelocation + filename.substr(1);
		}

		std::cout << "[WebSocketServer_IF][INFO] HTTP request on content: '" << filename << "'." << std::endl;

		file.open(filePath.c_str(), std::ios::in);
		if (!file) {
			std::cout << "[WebSocketServer_IF][WARN] HTTP content could not be found. Serving 404 instead. Missing content: '" << filename << "'." << std::endl;
			// 404 error
			std::stringstream ss;

			ss << "<!doctype html><html><head>"
				<< "<title>Error 404 (Resource not found)</title><body>"
				<< "<h1>Error 404</h1>"
				<< "<p>The requested URL " << filename << " was not found on this server.</p>"
				<< "</body></head></html>";

			con->set_body(ss.str());
			con->set_status(websocketpp::http::status_code::not_found);
			return;
		}

		file.seekg(0, std::ios::end);
		response.reserve(file.tellg());
		file.seekg(0, std::ios::beg);

		response.assign((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		con->set_body(response);
		con->set_status(websocketpp::http::status_code::ok);

	}

	void readConfigfile() {
		try {
			std::ifstream configStream("config.json");
			json config;
			if (configStream.is_open()) {
				std::cout << "[WebSocketServer_IF][INFO] Configurationfile found." << std::endl;
				configStream >> config;
				configStream.close();

				if (config.is_object())
				{
					// Get broadcastinterval
					if (config["WebSocketServer_IF"]["port"].is_number())
					{
						port = config["WebSocketServer_IF"]["port"];
						std::cout << "[WebSocketServer_IF][INFO] Serverport changed to '" << port << "'." << std::endl;
					}
					else
					{
						std::cout << "[WebSocketServer_IF][WARN] Syntaxerror in configurationfile." << std::endl;
					}

					// Get httpfile location
					if (config["WebSocketServer_IF"]["httpfilelocation"].is_string())
					{
						httpfilelocation = config["WebSocketServer_IF"]["httpfilelocation"];
						std::cout << "[WebSocketServer_IF][INFO] Httpfile location changed to '" << httpfilelocation << "'." << std::endl;
					}
					else
					{
						std::cout << "[WebSocketServer_IF][WARN] Syntaxerror in configurationfile" << std::endl;
					}


					// Add more config here.


					std::cout << "[WebSocketServer_IF][INFO] End of configurationfile." << std::endl;
				}
				else
				{
					std::cout << "[WebSocketServer_IF][INFO] Configurationfile is empty." << std::endl;
				}
			}
			else
			{
				std::cout << "[WebSocketServer_IF][INFO] Configurationfile was not found." << std::endl;
			}
		}
		catch (const std::exception& ex)
		{
			std::cout << "[WebSocketServer_IF][WARN] An error occurred while reading configurationfile." << std::endl;
			std::cout << "[WebSocketServer_IF][WARN] " << ex.what() << std::endl;
		}
	}
};