#include "UserData_Storage.hpp"

UserData_Storage::UserData_Storage(/* args */)
{
}

UserData_Storage::~UserData_Storage()
{
}

void UserData_Storage::UpdatePlayerScore(int player)
{
    playerScore[player-1]++;
}
