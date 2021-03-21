#include "includes/rps.hpp"
#include "sleepy_discord/sleepy_discord.h"

using namespace std;

namespace testbot{
    void RPS::startGame(){
        state = LOADING;
        client->sendMessage(channel,"Starting a rock,paper,session with " + player.username +"!");
        SleepyDiscord::Message message = client->sendMessage(channel,"Game starts in:");
        client->schedule([this,message](){
             client->editMessage(message,"3!");
        },1000);
        client->schedule([this,message](){
             client->editMessage(message,"2!");
        },2000);
        client->schedule([this,message](){
             client->editMessage(message,"1!");
        },3000);
        client->schedule([this,message](){
             client->editMessage(message,"Start!");
        },4000);
        state = GAME;
        cout << getState() << endl;
    }
    RPS::State RPS::getState(){
        return state;
    }

    RPS::winState RPS::playerVsBot(){
        Weapon botChoiceInt = (Weapon) (rand() % 3);
        return (winState) ((playerChoice - botChoiceInt + 3)%3);
    }
}
