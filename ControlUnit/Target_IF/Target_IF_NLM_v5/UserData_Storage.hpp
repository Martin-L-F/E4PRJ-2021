
#include <iostream>
using namespace std;

class UserData_Storage
{
public:
    UserData_Storage(/* args */){}
    ~UserData_Storage() {}
    void UpdatePlayerScore(int player)
    {
        playerScore[player - 1]++;
        cout << "Player " << player << " now has " << playerScore[player - 1] << " points" << endl;
    }
private:
    int playerScore[2] = {};
};

