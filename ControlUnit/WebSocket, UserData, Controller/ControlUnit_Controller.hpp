#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <thread>
#include "WebSocketServer_IF/WebSocketServer_IF.h"
#include "UserData_Storage/UserData_Storage.hpp"

class ControlUnit_Controller
{
public:
	ControlUnit_Controller()
	{
		server.add_onMessage_Handler(std::bind(&ControlUnit_Controller::onWebSocketMessage, this, std::placeholders::_1));
	}

	void run()
	{
		std::cout << std::endl
				  << " ============================== " << std::endl
				  << " === TargetBeam ControlUnit === " << std::endl
				  << " ============================== " << std::endl
				  << std::endl;
		server.run();
	}

private:
	// Transmits all data for signed in users to the primary browser.
	void broadcastSignedInUsers()
	{
		auto res = server.getConnectionsOfType(Client::connectionType::primaryBrowser);
		if (res.size() > 0)
		{
			json message;
			message["type"] = "SignedInUsers";
			message["data"] = json::array();
			for (auto &user : signedInUsers)
			{
				json item;
				item = user.user->jsondump();
				item["curentScore"] = user.curentScore;
				if (user.isWinner)
				{
					item["isWinner"] = true;
				}
				message["data"].push_back(item);
			}
			res[0]->send(message);
		}
	}

	// Handler for incomming WebSocket messages.
	void onWebSocketMessage(Message *message)
	{
		if (message->getContent()["type"] == "RequestConnectionVerification")
		{
			if (message->getContent()["connectionType"] == "browser")
			{
				if (server.getConnectionsOfType(Client::connectionType::primaryBrowser).size() > 0)
				{
					server.redefineConnection(message->getClient(), Client::connectionType::dormantBrowser);
					json res;
					res["data"] = "SetToDormantBrowser";
					message->addToResponse(res);
				}
				else
				{
					server.redefineConnection(message->getClient(), Client::connectionType::primaryBrowser);
					json res;
					res["data"] = "UpgradeToPrimaryBrowser";
					message->addToResponse(res);
					broadcastSignedInUsers();
				}
				message->addToResponse("{\"status\": 200}"_json);
			}
			else if (message->getContent()["connectionType"] == "pistol")
			{
				if (message->getContent()["battery"].is_number() && message->getContent()["charging"].is_boolean())
				{
					server.redefineConnection(message->getClient(), Client::connectionType::pistol, false);
					json meta;
					meta["battery"] = message->getContent()["battery"];
					meta["charging"] = message->getContent()["charging"];
					message->getClient()->mergeMetadata(meta);
					server.broadcastConnections();
					json res;
					res["status"] = 200;
					res["data"] = "UpgradeToPistol";
					message->addToResponse(res);
				}
				else
				{
					std::cout << "[ControlUnit_Controller][WARN] Unresolved message. Error in data for message of type: RequestConnectionVerification.pistol" << std::endl;
					message->addToResponse("{\"status\": 400}"_json);
				}
			}
			else
			{
				std::cout << "[ControlUnit_Controller][WARN] Unresolved message. Error in data for message of type: RequestConnectionVerification" << std::endl;
				message->addToResponse("{\"status\": 400}"_json);
			}
		}

		else if (message->getContent()["type"] == "BatteryUpdate")
		{
			if (message->messageRequiresType(Client::connectionType::pistol))
				return;

			if (message->getContent()["battery"].is_number() && message->getContent()["charging"].is_boolean())
			{
				json meta;
				meta["battery"] = message->getContent()["battery"];
				meta["charging"] = message->getContent()["charging"];
				message->getClient()->mergeMetadata(meta);
				server.broadcastConnections();
				message->addToResponse("{\"status\": 200}"_json);
			}
			else
			{
				std::cout << "[ControlUnit_Controller][WARN] Unresolved message. Error in data for message of type: BatteryUpdate" << std::endl;
				message->addToResponse("{\"status\": 400}"_json);
			}
		}

		else if (message->getContent()["type"] == "RequestSignin")
		{
			if (message->messageRequiresType(Client::connectionType::primaryBrowser))
				return;

			if (message->getContent()["email"].is_string() && message->getContent()["password"].is_string())
			{

				json res;
				int status = storage.trySignin(message->getContent()["email"], message->getContent()["password"]);
				res["status"] = status;
				if (status == 200)
				{
					for (auto &user : signedInUsers)
					{
						if (user.user->getEmail() == message->getContent()["email"])
						{
							message->addToResponse("{\"status\": 208 }"_json);
							std::cout << "[ControlUnit_Controller][WARN] User tried to sign in a user that is already signed in." << std::endl;
							return;
						}
					}
					signedInUsers.push_back({storage.getUser(message->getContent()["email"]), 0});
					broadcastSignedInUsers();
				}

				message->addToResponse(res);
			}
			else
			{
				std::cout << "[ControlUnit_Controller][WARN] Unresolved message. Error in data for message of type: RequestSignin" << std::endl;
				message->addToResponse("{\"status\": 400}"_json);
			}
		}

		else if (message->getContent()["type"] == "RequestSignOut")
		{
			if (message->messageRequiresType(Client::connectionType::primaryBrowser))
				return;

			if (message->getContent()["email"].is_string())
			{

				for (auto user = signedInUsers.begin(); user < signedInUsers.end(); user++)
				{
					if ((*user).user->getEmail() == message->getContent()["email"])
					{
						message->addToResponse("{\"status\": 200}"_json);
						signedInUsers.erase(user);
						broadcastSignedInUsers();
						return;
					}
				}
				message->addToResponse("{\"status\": 400}"_json);
				std::cout << "[ControlUnit_Controller][WARN] Unresolved message. Error in data for message of type: RequestSignOut. Email not found in signedin-list" << std::endl;
			}
			else
			{
				std::cout << "[ControlUnit_Controller][WARN] Unresolved message. Error in data for message of type: RequestSignOut" << std::endl;
				message->addToResponse("{\"status\": 400}"_json);
			}
		}

		else if (message->getContent()["type"] == "RequestChangeName")
		{
			if (message->messageRequiresType(Client::connectionType::primaryBrowser))
				return;

			if (message->getContent()["email"].is_string() && message->getContent()["name"].is_string())
			{
				json res;
				res["status"] = storage.updateName(message->getContent()["email"], message->getContent()["name"]);
				message->addToResponse(res);

				if (res["status"] == 200)
				{
					for (auto &user : signedInUsers)
					{
						if (user.user->getEmail() == message->getContent()["email"])
						{
							broadcastSignedInUsers();
							return;
						}
					}
				}
			}
			else
			{
				std::cout << "[ControlUnit_Controller][WARN] Unresolved message. Error in data for message of type: RequestChangeName" << std::endl;
				message->addToResponse("{\"status\": 400}"_json);
			}
		}

		else if (message->getContent()["type"] == "RequestChangePassword")
		{
			if (message->messageRequiresType(Client::connectionType::primaryBrowser))
				return;

			if (message->getContent()["email"].is_string() && message->getContent()["oldPassword"].is_string() && message->getContent()["newPassword"].is_string())
			{
				json res;
				res["status"] = storage.updatePassword(message->getContent()["email"], message->getContent()["oldPassword"], message->getContent()["newPassword"]);
				message->addToResponse(res);
			}
			else
			{
				std::cout << "[ControlUnit_Controller][WARN] Unresolved message. Error in data for message of type: RequestChangePassword" << std::endl;
				message->addToResponse("{\"status\": 400}"_json);
			}
		}

		else if (message->getContent()["type"] == "RequestCreateUser")
		{
			if (message->messageRequiresType(Client::connectionType::primaryBrowser))
				return;

			if (message->getContent()["name"].is_string() && message->getContent()["email"].is_string() && message->getContent()["password"].is_string())
			{
				json res;
				res["status"] = storage.CreateUser(message->getContent()["name"], message->getContent()["email"], message->getContent()["password"]);
				message->addToResponse(res);
				if (res["status"] == 200)
				{
					signedInUsers.push_back({storage.getUser(message->getContent()["email"]), 0});
					broadcastSignedInUsers();
				}
			}
			else
			{
				std::cout << "[ControlUnit_Controller][WARN] Unresolved message. Error in data for message of type: RequestChangePassword" << std::endl;
				message->addToResponse("{\"status\": 400}"_json);
			}
		}

		else if (message->getContent()["type"] == "ReadyGame")
		{
			if (message->messageRequiresType(Client::connectionType::primaryBrowser))
				return;
			message->addToResponse("{\"status\": 501}"_json);
			readyAllPistols();

			/*
			  TO-DO: Add more functionality here...
			*/
		}

		else if (message->getContent()["type"] == "StartGame")
		{
			if (message->messageRequiresType(Client::connectionType::primaryBrowser))
				return;

			if (message->getContent()["difficulty"].is_string())
			{
				if (message->getContent()["difficulty"] == "easy" || message->getContent()["difficulty"] == "medium" || message->getContent()["difficulty"] == "hard")
				{

					if (server.getConnectionsOfType(Client::connectionType::pistol).size() <= 0)
					{
						message->addToResponse("{\"status\": 403}"_json);
						message->addToResponse("{\"reason\": \"No pistols found\"}"_json);
						std::cout << "[ControlUnit_Controller][WARN] Tried to start game without a pistol connected. Gamestart terminated." << std::endl;
						return;
					}

					if (signedInUsers.size() <= 0)
					{
						message->addToResponse("{\"status\": 403}"_json);
						message->addToResponse("{\"reason\": \"No users found\"}"_json);
						std::cout << "[ControlUnit_Controller][WARN] Tried to start game without a user signed in. Gamestart terminated." << std::endl;
						return;
					}

					if (signedInUsers.size() > server.getConnectionsOfType(Client::connectionType::pistol).size())
					{
						message->addToResponse("{\"status\": 403}"_json);
						message->addToResponse("{\"reason\": \"There are more users than there are pistols\"}"_json);
						std::cout << "[ControlUnit_Controller][WARN] Tried to start game with more users than pistols.. Gamestart terminated." << std::endl;
						return;
					}

					currentGameDifficulty = message->getContent()["difficulty"];
					armAllPistols(currentGameDifficulty);
					message->addToResponse("{\"status\": 200}"_json);

					/*
					  TO-DO: Add more functionality here...
					*/
				}
				else
				{
					std::cout << "[ControlUnit_Controller][WARN] Unresolved message. Error in content 'difficulty' for message of type: StartGame. Content is: '" << message->getContent()["difficulty"] << "'." << std::endl;
					message->addToResponse("{\"status\": 400}"_json);
				}
			}
			else
			{
				std::cout << "[ControlUnit_Controller][WARN] Unresolved message. Error in data for message of type: StartGame" << std::endl;
				message->addToResponse("{\"status\": 400}"_json);
			}
		}

		else if (message->getContent()["type"] == "CountdownOver")
		{
			if (message->messageRequiresType(Client::connectionType::primaryBrowser))
				return;
			message->addToResponse("{\"status\": 501}"_json);

			/*
			  TO-DO: Add more functionality here...
			*/
		}

		else if (message->getContent()["type"] == "")
		{
		}

		else if (message->getContent()["type"] == "")
		{
		}

		else
		{
			std::cout << "[ControlUnit_Controller][WARN] Unresolved message type" << std::endl;
			message->addToResponse("{\"status\": 400}"_json);
		}
	}

	// Transmits a message to all pistols that the game has started.
	void armAllPistols(std::string difficulty)
	{
		auto res = server.getConnectionsOfType(Client::connectionType::pistol);
		if (res.size() > 0)
		{
			json message;
			message["type"] = "armPistol";
			message["difficulty"] = difficulty;
			message["frequency"]["range"] = signedInUsers.size();
			int index = 0;
			for (auto &client : res)
			{
				message["frequency"]["index"] = index++;
				client->send(message);

				if (index >= signedInUsers.size())
				{
					return;
				}
			}
		}
	}

	// Transmits a message to all pistols that prevents standby.
	void readyAllPistols()
	{
		auto res = server.getConnectionsOfType(Client::connectionType::pistol);
		if (res.size() > 0)
		{
			json message;
			message["type"] = "preventStandby";
			for (auto &client : res)
			{
				client->send(message);
			}
		}
	}

	// Transmits a message to all pistols that the game is over.
	void disarmAllPistols()
	{
		auto res = server.getConnectionsOfType(Client::connectionType::pistol);
		if (res.size() > 0)
		{
			json message;
			message["type"] = "disarmPistol";
			for (auto &client : res)
			{
				client->send(message);
			}
		}
	}

	// Disable all pistols and make primary browser show statistics.
	void gameIsOver()
	{
		disarmAllPistols();

		auto res = server.getConnectionsOfType(Client::connectionType::primaryBrowser);
		if (res.size() > 0)
		{
			json message;
			message["type"] = "ShowStats";
			res[0]->send(message);
		}

		for (auto &user : signedInUsers)
		{
			user.user->updateScore(user.curentScore, currentGameDifficulty);
			user.isWinner = false;
		}
		currentGameDifficulty = "";
	}

	// Add points to a signed in user based on the users location in the list.
	void updateScore(int userIndex, int pointsToAdd)
	{
		if (userIndex >= signedInUsers.size() && userIndex >= 0)
		{
			std::cout << "[ControlUnit_Controller][ERROR] index of user is out of range. Range is: " << signedInUsers.size() << ". Index is: " << userIndex << "." << std::endl;
			return;
		}
		signedInUsers[userIndex].curentScore += pointsToAdd;

		if (signedInUsers[userIndex].curentScore >= maxPoints)
		{
			signedInUsers[userIndex].isWinner = true;
			broadcastSignedInUsers();
			gameIsOver();
		}
		else
		{
			broadcastSignedInUsers();
		}
	}

	WebSocketServer_IF server;
	UserData_Storage storage;
	struct SignedInUser
	{
		User_t *user = nullptr;
		int curentScore = 0;
		bool isWinner = false;
	};
	std::vector<SignedInUser> signedInUsers;
	std::string currentGameDifficulty = "";
	int maxPoints = 100;
};