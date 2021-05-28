#pragma once
#include <iostream>
#include <fstream>
#include <ctime>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>

#include "json.hpp"
#include "User_t.hpp"

using json = nlohmann::json;


class Lock {
public:
	Lock(std::mutex* mutex) {
		this->mutex = mutex;
		mutex->lock();
	}
	~Lock() {
		mutex->unlock();
	}

private:
	std::mutex* mutex;
};

class UserData_Storage {
public:
	UserData_Storage() {
		readData();
	}

	~UserData_Storage() {
		if (databaseUpdate_Thread.joinable()) {
			databaseUpdate_Thread.join();
		}
	}

	/*
	Validate a email - password pair
	Returns:
	200 = sucess
	401 = password incorrect
	404 = user not found*/
	int trySignin(std::string email, std::string password) {
		int val = 0;
		for (auto& user : users) {
			val = user.trySignin(email, password);
			if (val != 404) return val;
		}
		return 404;
	}

	/*
	Get user-pointer from email.
	*/
	User_t* getUser(std::string email) {
		for (auto& user : users) {
			if (user.getEmail() == email) {
				return &user;
			}
		}
		std::cout << "[UserData_Storage][WARN] User with email: '" << email << "' could not be found" << std::endl;
		return nullptr;
	}

	/*
	Update user highscore from email
	difficulty: easy/medium/hard.
	Returns:
	200 = sucess
	400 = invalid difficulty
	404 = user not found*/
	int updateScore(std::string email, int score, std::string difficulty) {
		User_t* user = getUser(email);
		if (user == nullptr) {
			return 404;
		}
		return updateScore(user, score, difficulty);
	}

	/*
	Update user password from email
	Returns:
	200 = sucess
	401 = old password incorrect
	404 = user not found*/
	int updatePassword(std::string email, std::string oldPassword, std::string newPassword) {
		User_t* user = getUser(email);
		if (user == nullptr) {
			return 404;
		}
		return updatePassword(user, oldPassword, newPassword);
	}

	/*
	Update user name from email
	Returns:
	200 = sucess
	400 = invalid
	404 = user not found*/
	int updateName(std::string email, std::string newName) {
		User_t* user = getUser(email);
		if (user == nullptr) {
			return 404;
		}
		return updateName(user, newName);
	}

	/*
	Create a new user
	Returns:
	200 = sucess
	400 = email already exists*/
	int CreateUser(std::string name, std::string email, std::string password, json metadata = nullptr) {
		if (emailExists(email)) {
			return 400;
		}
		{
			Lock lock(&usersMutex);
			users.push_back(User_t(name, email, password, metadata));
		}
		updateDatabase();
		return 200;
	}

private:
	// Vector of users. Secured by std::mutex usersMutex
	std::vector<User_t> users;

	// Mutex securing std::vector<User_t> users
	std::mutex usersMutex;

	// Location of datafile
	std::string filePath = "UserData_DatabaseFile.json";

	/*
	Update user highscore from pointer.
	difficulty: easy/medium/hard.
	Returns:
	200 = sucess
	400 = invalid difficulty*/
	int updateScore(User_t* user, int score, std::string difficulty) {
		int status;
		{
			Lock lock(&usersMutex);
			status = user->updateScore(score, difficulty);
		}
		updateDatabase();
		return status;
	}

	/*
	Update user name from pointer
	Returns:
	200 = sucess
	400 = invalid*/
	int updateName(User_t* user, std::string newName) {
		{
			Lock lock(&usersMutex);

			if (user->setName(newName) == 400) {
				return 400;
			}
		}
		updateDatabase();
		return 200;
	}

	/*
	Update user password from pointer
	Returns:
	200 = sucess
	401 = old password incorrect*/
	int updatePassword(User_t* user, std::string oldPassword, std::string newPassword) {
		{
			Lock lock(&usersMutex);

			if (user->updatePassword(oldPassword, newPassword) == 401) {
				return 401;
			}
		}
		updateDatabase();
		return 200;
	}








	//Reads users from file and places in variables
	void readData() {
		try {
			std::ifstream datafile;
			datafile.open(filePath);
			if (datafile.is_open()) {
				json data;
				datafile >> data;
				datafile.close();
				if (!data.is_object() || !data["users"].is_array()) {
					std::cout << "[UserData_Storage][ERROR] Syntax error in userdata." << std::endl;
					return;
				}
				int i = 0;
				int sucess = 0;
				for (auto& user : data["users"]) {
					i++;
					if (emailExists(user["name"])) {
						std::cout << "[UserData_Storage][ERROR] Syntax error in userdata at user " << i << ". Email is already loaded." << std::endl;
						break;
					}
					User_t newUser(user);
					if (newUser.getEmail() != "") {
						Lock lock(&usersMutex);
						users.push_back(newUser);
						sucess++;
					}
				}
				std::cout << "[UserData_Storage][INFO] found " << sucess << " users in userdatafile on " << i << " lines." << std::endl;
			}
			else {
				std::cout << "[UserData_Storage][ERROR] File could not be opened." << std::endl;
			}
			datafile.close();
		}
		catch (const std::exception& ex)
		{
			std::cout << "[UserData_Storage][WARN] An error occurred while reading userdata." << std::endl;
			std::cout << "[UserData_Storage][WARN] " << ex.what() << std::endl;
		}
	}

	/*
	Checks if email is in local variables
	Returns:
	true = email exists
	false = email does not exist*/
	bool emailExists(std::string email) {
		Lock lock(&usersMutex);
		for (auto& user : users) {
			if (user.getEmail() == email) {
				return true;
			}
		}
		return false;
	}

	//Thread to execute void _updateDatabase().
	std::thread databaseUpdate_Thread;

	// Flag indicating local variables are newer than the database file and a datasync is required.
	bool updateRequired = false;
	// Flag indicationg datasync-thread is not running.
	bool updateComplete = true;

	// Saves variables in file. Async.
	void updateDatabase() {
		updateRequired = true;
		if (updateComplete) {
			updateComplete = false;
			if (databaseUpdate_Thread.joinable()) {
				databaseUpdate_Thread.join();
			}
			databaseUpdate_Thread = std::thread(std::bind(&UserData_Storage::_updateDatabase, this));
		}
	}

	// Function to run in std::thread databaseUpdate_Thread.
	void _updateDatabase() {
		do {
			updateRequired = false;
			// Serialize data
			json data;
			try {
				Lock lock(&usersMutex);
				for (auto& user : users) {
					data["users"].push_back(user.jsondumpSecure());
				}
			}
			catch (const std::exception& ex)
			{
				std::cout << "[UserData_Storage][ERROR] An error occurred while serializing userdata for storagedump." << std::endl;
				std::cout << "[UserData_Storage][ERROR] " << ex.what() << std::endl;
			}

			// Save data
			std::ofstream datafile;
			try {
				datafile.open(filePath);
				if (datafile.is_open()) {
					datafile << data;
					datafile.close();
				}
				else {
					std::cout << "[UserData_Storage][ERROR] An error occurred while trying to perform storagedump to file. File could not be opened." << std::endl;
					std::cout << "[UserData_Storage][ERROR] Data will be dumped in log." << std::endl
						<< "----------------------------------" << std::endl
						<< data.dump() << std::endl
						<< "----------------------------------" << std::endl;
				}
				datafile.close();
			}
			catch (const std::exception& ex)
			{
				datafile.close();
				std::cout << "[UserData_Storage][ERROR] An error occurred while trying to perform storagedump to file." << std::endl;
				std::cout << "[UserData_Storage][ERROR] " << ex.what() << std::endl;
				std::cout << "[UserData_Storage][ERROR] Data will be dumped in log." << std::endl
					<< "----------------------------------" << std::endl
					<< data.dump() << std::endl
					<< "----------------------------------" << std::endl;
			}
		} while (updateRequired);
		updateComplete = true;
		std::cout << "[UserData_Storage][INFO] Datafile updated." << std::endl;
	}
};