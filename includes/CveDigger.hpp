#pragma once
#ifndef CVEDIGGER_HPP
#define CVEDIGGER_HPP

#include <curl/curl.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>
#include "sleepy_discord/sleepy_discord.h"
#include <vector>
#include <nlohmann/json.hpp>

using std::string;
using std::vector;
using std::cout;
using namespace sql;
using json = nlohmann::json;
namespace testbot
{
    class CveDigger
    {
    private: 
        sql::Statement *stmt;
        sql::ResultSet *res;
        sql::PreparedStatement *prep_stmt;
        string BASEAPIURL = "https://services.nvd.nist.gov/rest/json/cves/1.0?";
        CURL *curl;
        SleepyDiscord::DiscordClient *client;
        void getConnection(sql::Connection *conn);
        void getCVEs();
        std::string formatTime(long time);
        static size_t write_to_string(void *ptr, size_t size, size_t count, void *stream);
        json getData(json cves,json cleaned);

    public:
        CveDigger(){};
        void setClient(SleepyDiscord::DiscordClient *client);
        void run();
    };

   
} // namespace testbot

#endif
