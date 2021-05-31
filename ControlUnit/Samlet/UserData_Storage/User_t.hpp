#pragma once
#include <iostream>
#include <fstream>
#include <mutex>

#include "json.hpp"

using json = nlohmann::json;


class User_t {
public:
	// Default constructor for user_t
	User_t() {

	}

	// Constructor with values for user_t
	User_t(std::string name, std::string email, std::string password, int score_easy, int score_medium, int score_hard, json metadata = NULL) {
		this->name = name;
		this->email = email;
		this->password = password;
		this->score_easy = score_easy;
		this->score_medium = score_medium;
		this->score_hard = score_hard;
		this->metadata = metadata;
	}

	// Constructor with values for user_t
	User_t(std::string name, std::string email, std::string password, json metadata = nullptr) {
		this->name = name;
		this->email = email;
		this->password = password;
		this->metadata = metadata;
	}

	// Constructor with json for user_t
	User_t(json data) {
		if (!data["name"].is_string() || !data["email"].is_string() || !data["password"].is_string() || !data["highscore"].is_object() || !data["highscore"]["easy"].is_number() || !data["highscore"]["medium"].is_number() || !data["highscore"]["hard"].is_number()) {
			std::cout << "[UserData_Storage][ERROR] Syntax error in jsonpass for User_t" << std::endl;
			return;
		}
		name = data["name"];
		email = data["email"];
		password = data["password"];
		score_easy = data["highscore"]["easy"];
		score_medium = data["highscore"]["medium"];
		score_hard = data["highscore"]["hard"];
		metadata = data["metadata"];
	}

	// Returns data in json form.
	json jsondump() {
		json user_json;
		user_json["name"] = name;
		user_json["email"] = email;

		user_json["highscore"]["easy"] = score_easy;
		user_json["highscore"]["medium"] = score_medium;
		user_json["highscore"]["hard"] = score_hard;
		user_json["metadata"] = metadata;

		return user_json;
	}

	// Returns data in json form. Includes password.
	json jsondumpSecure() {
		json user_json;
		user_json["name"] = name;
		user_json["email"] = email;
		user_json["password"] = password;

		user_json["highscore"]["easy"] = score_easy;
		user_json["highscore"]["medium"] = score_medium;
		user_json["highscore"]["hard"] = score_hard;
		user_json["metadata"] = metadata;

		return user_json;
	}

	/*
	Validate a email - password pair
	Returns:
	200 = sucess
	401 = password incorrect
	404 = email incorrect*/
	int trySignin(std::string email, std::string password) {
		if (this->email == email) {
			if (this->password == password) {
				return 200;
			}
			else {
				return 401;
			}
		}
		else {
			return 404;
		}
	}

	// Returns email.
	std::string getEmail() {
		return email;
	}

	// Returns name.
	std::string getName() {
		return name;
	}

	/*Returns highscore of given difficulty.
	difficulty = easy/medium/hard*/
	int getHighscore(std::string difficulty) {
		if (difficulty == "easy") {
			return score_easy;
		}
		else if (difficulty == "medium") {
			return score_medium;
		}
		else if (difficulty == "hard") {
			return score_hard;
		}
		else {
			std::cout << "[UserData_Storage][WARN] An error occurred trying to get user score. Unknown difficulty: '" << difficulty << "'" << std::endl;
			return NULL;
		}
	}

	/*
	Update user name.
	Returns:
	200 = sucess
	400 = invalid*/
	int setName(std::string name) {
		if (name != "") {
			this->name = name;
			return 200;
		}
		return 400;
		
	}

	/*
	Update user highscore.
	difficulty: easy/medium/hard.
	Returns:
	200 = sucess
	400 = invalid difficulty*/
	int updateScore(int score, std::string difficulty) {
		if (difficulty == "easy") {
			score_easy = (score_easy < score ? score : score_easy);
			return 200;
		}
		else if (difficulty == "medium") {
			score_medium = (score_medium < score ? score : score_medium);
			return 200;
		}
		else if (difficulty == "hard") {
			score_hard = (score_hard < score ? score : score_hard);
			return 200;
		}
		else {
			std::cout << "[UserData_Storage][WARN] An error occurred trying to set user score. Unknown difficulty: '" << difficulty << "'" << std::endl;
			return 400;
		}
	}

	/*
	Update user password.
	Returns:
	200 = sucess
	401 = old password incorrect*/
	int updatePassword(std::string oldPassword, std::string newPassword) {
		if (password != oldPassword) {
			return 401;
		}
		password = newPassword;
		return 200;
	}

private:
	std::string name = "";
	std::string email = "";
	std::string password = "";
	int score_easy = 0;
	int score_medium = 0;
	int score_hard = 0;
	json metadata = nullptr;
};