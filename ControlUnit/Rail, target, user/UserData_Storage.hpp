#pragma once
#include <iostream>
using namespace std;

//Purely made for Target_IF test. Just change it.
class UserData_Storage
{
public:
    UserData_Storage(){}
    ~UserData_Storage() {}
    void UpdatePlayerScore(int player)
    {
        playerScore[player - 1]++;
        cout << "Player " << player << " now has " << playerScore[player - 1] << " points" << endl;
    }
private:
    int playerScore[2] = {};
};

