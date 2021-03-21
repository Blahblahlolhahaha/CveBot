#pragma once
#ifndef RPS_HPP
#define DPS_HPP

#include<string>
#include<iostream>
#include<vector>
#include "sleepy_discord/sleepy_discord.h"

namespace testbot{
    struct RPS{
        enum Weapon{
            ROCK = 0,
            PAPER = 1,
            SCISSORS = 2,
        };

        enum winState {
            WIN  = 1,
            TIE  = 0,
            LOSE = 2
        };

        enum State{
            STOP = 0,
            LOADING = 1,
            GAME = 2,
            ENDED = 3
        };

        State state = STOP;
        SleepyDiscord::User player;
        SleepyDiscord::DiscordClient *client;
        SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel;
        Weapon playerChoice;
        
        RPS(){}
        RPS(SleepyDiscord::User player,SleepyDiscord::DiscordClient* client,SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel):
        player(player),
        client(client),
        channel(channel)
        {}
        
        void startGame();
        State getState();
        winState playerVsBot();
    };
}

#endif