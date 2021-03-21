// testbot.cpp : Defines the entry point for the application.
//
// bot url=https://discord.com/api/oauth2/authorize?client_id=805438560164773919&permissions=3668032&scope=bot
#include "sleepy_discord/sleepy_discord.h"
#include "includes/rps.hpp"
#include "includes/CveDigger.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <curl/curl.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
using namespace std;
using namespace testbot;

class TestBotClient : public SleepyDiscord::DiscordClient {	
	private:
		vector<RPS> games;
		RPS currentGame;
	public:
		using SleepyDiscord::DiscordClient::DiscordClient;
		enum commands {
			echo,
			rps,
			subscribe,
			nulll,
		};
		void onServer(SleepyDiscord::Server server){
			for(SleepyDiscord::Channel channel : server.channels){
				if(channel.type == SleepyDiscord::Channel::SERVER_TEXT){
					sendMessage(channel.ID,"sup");
					break;
				}
			}
		}

		void onMessage(SleepyDiscord::Message message)override {	
			if (message.startsWith("!sad")) {
				string command = message.content.substr(4);
				if (command.rfind(" ", 0) == 0) {
					command = command.substr(1);
				}	
				string order = getCommand(command);
				switch (getEnum(order)) {
					case echo:{
						string echoMessage = getParams(command);
						sendMessage(message.channelID, echoMessage);
						break;
					}
					case rps:{
						testbot::RPS game(message.author,this,message.channelID);
						games.push_back(game);
						if(games.size() == 1){
							currentGame = game;
							currentGame.startGame();
						}
						break;
					}
					case subscribe:{
						try{
							sql::Driver *driver = get_driver_instance();
							sql::Connection *conn = driver->connect("tcp://127.0.0.1:3306","pikachu","password");

							string dbName = "Subscribers";
							sql::Statement *createDB = conn->createStatement();
							createDB->execute("Use " + dbName);
							
							sql::PreparedStatement *checkServer = conn->prepareStatement("Select * from servers where serverID = ?");
							checkServer->setBigInt(1,to_string(message.serverID.number()));
							sql::ResultSet *res = checkServer->executeQuery();
							if(res->next()){
								sql::PreparedStatement *createSuscriber = conn->prepareStatement("update servers set channelID = ? where serverID= ?");
								createSuscriber->setBigInt(1,to_string(message.channelID.number()));
								createSuscriber->setBigInt(2,to_string(message.serverID.number()));
								createSuscriber->execute();
								delete createSuscriber;
							}
							else{
								sql::PreparedStatement *createSuscriber = conn->prepareStatement("Insert into servers(serverID,channelID) VALUES(?,?)");
								createSuscriber->setBigInt(1,to_string(message.serverID.number()));
								createSuscriber->setBigInt(2,to_string(message.channelID.number()));
								createSuscriber->execute();
								delete createSuscriber;
							}
							delete res;
							delete checkServer;
							delete createDB;
							delete conn;
							sendMessage(message.channelID,"Hooray sucess! :D");
						}catch(sql::SQLException){
							sendMessage(message.channelID,"An error occured T.T!");
						}
					}
				}
			}
			else if (message.author == currentGame.player){
				switch (currentGame.getState()){
				case testbot::RPS::GAME:{
					if(message.content == "rock")currentGame.playerChoice = testbot::RPS::ROCK;
					else if(message.content == "paper")currentGame.playerChoice = testbot::RPS::PAPER;
					else if(message.content == "scissors")currentGame.playerChoice = testbot::RPS::SCISSORS;
					cout << message.content << endl;
					RPS::winState results = currentGame.playerVsBot();
					switch (results)
					{
					case RPS::WIN:
						sendMessage(message.channelID,"I lost T.T" );
						break;
					case RPS::TIE:
						sendMessage(message.channelID,"Ohhh, we chose the same option!");
						break;
					
					case RPS::LOSE:
						sendMessage(message.channelID,"Yayyyyy I won! >.<");
						break;
					}
					currentGame.state = RPS::ENDED;
					schedule([this,message](){
						games.erase(games.begin());
						if(!games.empty()){
							currentGame = games[0];
							currentGame.startGame();
						}
					},1000);
					break;
				}
				case testbot::RPS::LOADING:{
					if(message.channelID == currentGame.channel){
						sendMessage(message.channelID,"Game haven't load finish yet! Don't rush me >^<");
					}
					break;
				}
				}
			}
		}
		commands getEnum(string sad) {
			if (sad == "echo") return echo;	
			else if(sad == "rps") return rps;
			else if(sad == "subscribe") return subscribe;
			return nulll;
		}

		string getCommand(string content) {
			cout << content << endl;
			int firstSpace = content.find(" ");
			if (firstSpace == string::npos) {
				return content;
			}
			string command = content.substr(0, firstSpace);
			return command;
		}
		string getParams(string content){
			int firstSpace = content.find(" ");
			string param = content.substr(firstSpace);
			return param;
		}
};

void initialiseDB(){
	sql::Driver *driver = get_driver_instance();
	string username = "user";
	string password = "password";
	sql::Connection *conn = driver->connect("tcp://127.0.0.1:3306",username,password);
	string dbName = "Subscribers";
	sql::Statement *createDB = conn->createStatement();	
	createDB->execute("CREATE DATABASE IF NOT EXISTS " + dbName);
	createDB->execute("Use " + dbName);
	createDB->execute("CREATE TABLE IF NOT EXISTS servers(serverID bigint NOT NULL, channelID bigint NOT NULL, primary key(serverID), unique key(channelID))");
	delete createDB;
	delete conn;
}

void runClient(TestBotClient *client){
	client->run();
}

void checkCve(TestBotClient *client){
	while(!client->isReady()){
		cout << "sad" << endl;
		sleep(1);
	}
	while(true){	
		CveDigger cveDigger = CveDigger();
		cveDigger.setClient(client);
		cveDigger.run(); 
		cout << "hi" << endl;
		sleep(3600);
	}
}

int main()
{
	string sad = "token";
	initialiseDB();
	TestBotClient client(sad, SleepyDiscord::USE_RUN_THREAD);
	thread t1(runClient,&client);
	thread t2(checkCve,&client);
	t1.join();
	t2.join();
	return 0;
}
