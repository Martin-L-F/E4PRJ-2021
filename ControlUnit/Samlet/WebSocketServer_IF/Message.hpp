#pragma once
#include "Client.hpp"


/*Message type represents an ingoing message and contains the message content as well as a pointer to the client that sent it.*/
class Message {
public:
	/*Default constructor. Not recommended.*/
	Message() {};

	/*Constructs a message using a std::string and a pointer to a client.
	The string will be JSON-passed.*/
	Message(std::string content, Client* client) {
		rawContent = content;
		try
		{
			this->content = json::parse(content);
		}
		catch (const std::exception& ex)
		{
			std::cout << "[WebSocketServer_IF][ERROR] Could not pass message content to JSON." << std::endl;
			std::cout << "[WebSocketServer_IF][ERROR] " << ex.what() << std::endl;
		}
		this->client = client;
		requestsReply = this->content["callback"].is_primitive() && !this->content["callback"].is_null();
		if (requestsReply) {
			response["type"] = "Callback";
			response["callback"] = this->content["callback"];
		}
	}

	/*Constructs a message using a std::string and a pointer to a client.
	The string will be JSON-passed.*/
	Message(json content, Client* client) {
		rawContent = content.dump();
		this->content = content;
		this->client = client;
		requestsReply = this->content["callback"].is_primitive() && !this->content["callback"].is_null();
		if (requestsReply) {
			response["type"] = "Callback";
			response["callback"] = this->content["callback"];
		}
	}

	/*Returns the entire message as a JSON-object*/
	json getContent() {
		return content;
	}

	/*Returns the entire message as std::string*/
	std::string getRawContent() {
		return rawContent;
	}

	/*Returns the type-field of the message in std::string form*/
	std::string getType() {
		return content["type"];
	}

	/*Returns a pointer to the client that sent the message*/
	Client* getClient() {
		return client;
	}

	/*Indicates wether or not a response will be sent to the client.
	Returns:
	true = a response will be sent
	false = a response will not be sent*/
	bool is_requestingReply() {
		return requestsReply;
	}

	// Returns wether or not the reply content has been altered.
	bool is_replyUpdated() {
		return replyHasBeenUpdated;
	}

	/*Merges a JSON-object with the response using merge_patch. Giving a value to the key "type" og "callback" will not take effect.
	Returns:
	200 = OK
	400 = merge error
	403 = Message is not requesting a response*/
	int addToResponse(json responseContent) {
		if (!requestsReply) return 403;
		try
		{
			json _responseContent = responseContent;
			_responseContent.merge_patch(R"({"type": null,"callback": null})"_json);
			response.merge_patch(_responseContent);
			replyHasBeenUpdated = true;
			return 200;
		}
		catch (const std::exception& ex)
		{
			std::cout << "[WebSocketServer_IF][ERROR] Could not merge replycontent." << std::endl;
			std::cout << "[WebSocketServer_IF][ERROR] " << ex.what() << std::endl;
			return 400;
		}
	}

	// Returns the response content as json.
	json getResponseContent() {
		return response;
	}

	// Returns true if the required type is not the correct type and fills response.
	bool messageRequiresType(Client::connectionType type) {
		if (client == nullptr) return false;

		if (client->getType() != type) {
			std::cout << "[WebSocketServer_IF][WARN] Connection of type: '" << client->getType() << "' tried to perform an action for connection of type: '" << type << "'. Terminating handle." << std::endl;
			addToResponse("{\"status\": 403}"_json);
			return true;
		}
		return false;
	}

private:
	std::string rawContent;
	json content;
	json response;
	Client* client = nullptr;
	bool requestsReply = false;
	bool replyHasBeenUpdated = false;

};