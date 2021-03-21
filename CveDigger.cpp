#include "includes/CveDigger.hpp"
#include <iostream>
#include <fmt/format.h>

using namespace sql;

namespace testbot{
    void CveDigger::setClient(SleepyDiscord::DiscordClient* client){
        this->client = client;
    }

    void CveDigger::run(){
        this->getCVEs();
    }
    string CveDigger::formatTime(long timee){
        string currentTime = std::ctime(&timee);
        string month,day,time,year;
        vector<string> months = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
        int spacePosition = currentTime.find(" ");
        while(spacePosition != string::npos){
            if(month.empty()){
                string name = currentTime.substr(spacePosition+1,3);
                for(int i=0;i<months.size();i++){
                    if(name == months[i]){
                        month = std::to_string(i + 1);
                        if(i < 10){
                            month = "0" + month;
                        }
                    }
                }
                
            }
            else if(day.empty()){
                day = currentTime.substr(spacePosition+1,2);
            }
            else if(time.empty()){
                time = currentTime.substr(spacePosition+1,8);
            }
            else if(year.empty()){
                year = currentTime.substr(spacePosition+1,4);
            }
            spacePosition = currentTime.find(" ",spacePosition+1);
        } 
        
        string combinedTime = year + "-" + month + "-" + day + 'T' + time + ":000%20" + "UTC%2B08:00";
        return combinedTime;
    }

    size_t CveDigger::write_to_string(void *ptr, size_t size, size_t count, void *stream){
        ((string *)stream)->append((char *)ptr, 0, size * count);
        return size * count;
    }

    json CveDigger::getData(json cves,json packed){
        packed = {};
        for(int i = 0; i<cves.size();i++){
            json cve = cves[i];
            json cleaned;
            cleaned["ID"] = cve["cve"]["CVE_data_meta"]["ID"];
            for(int x = 0; x < cve["cve"]["description"]["description_data"].size(); x++){
                json description = cve["cve"]["description"]["description_data"][x];
                if(description["lang"] == "en"){
                    cleaned["description"] = cve["cve"]["description"]["description_data"][0]["value"];
                    break;
                }
            }
            if(cleaned["description"] == "null"){
                continue;
            }
            json references = cve["cve"]["references"]["reference_data"];
            for(int y = 0; y<references.size();y++){
                json reference = references[i];
                cleaned["references"][y] = reference["url"];
            }
            packed["cves"][i] = cleaned;
        }
        return packed;
    }

    void CveDigger::getCVEs()
    {
        CURL *curl = curl_easy_init();
        if(curl){
            CURLcode res;
            string sad;
            auto current = std::chrono::system_clock::now();
            auto end = std::chrono::system_clock::to_time_t(current);
            auto start = end - (60*60);
            string startTime = formatTime(start);
            string endTime = formatTime(end);
            string finalUrl = BASEAPIURL + "modStartDate=" + startTime + "&modEndTime=" + endTime;
            curl_easy_setopt(curl, CURLOPT_URL,finalUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,write_to_string);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sad);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            json cves = json::parse(sad)["result"]["CVE_Items"];
            json packed = CveDigger::getData(cves,packed);
            for(int i = 0;i<packed["cves"].size();i++){
                json cve = packed["cves"][i];
                sql::Driver *driver = get_driver_instance();
                string username = "user";
                string password = "password";
                sql::Connection *conn = driver->connect("tcp://127.0.0.1:3306",username,password);

                string dbName = "Subscribers";
                sql::Statement *createDB = conn->createStatement();
                createDB->execute("Use " + dbName);

                sql::PreparedStatement *checkServer = conn->prepareStatement("Select * from servers");
                sql::ResultSet *res = checkServer->executeQuery();
                while(res->next()){
                    SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel = SleepyDiscord::Snowflake<SleepyDiscord::Channel>(res->getInt64("ChannelID"));
                    string message = fmt::v7::format("Description: {}\n\nReferences:\n",cve["description"]);
                    json references = cve["references"];
                    for(int x=0;x<references.size();x++){
                        if(references[x].is_null()){
                            continue;
                        }
                        string reference = references[x];
                        
                        message.append(reference+ "\n");
                    }
                    json embedJson;
                    embedJson["title"] = cve["ID"];
                    embedJson["description"] = message;
                    embedJson["color"] = 0x009dff;
                    string embedString = embedJson.dump();
                    SleepyDiscord::Embed embed = SleepyDiscord::Embed(embedString);
                    client->sendMessage(channel,"New CVE!",embed);
                }
            }
        }
        
    }

    
}
