#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include "database/database.hpp"

/*
This file get all data from Postgres Cluster by chunk, fetch data through SumAI, then use results to build SumDB.
*/

/*
Define a struct to store each row from postgres
*/

struct TopicNodeRow
{
    int id;
    std::string answer;
    std::string question;
    std::vector<std::string> keywords;
    std::time_t createdAt;
    std::time_t updatedAt;
};

struct SumDBRow
{
    int id;
    int chunkStart;
    int chunkEnd;
    std::string topic;
    std::string summary;
    std::time_t createdAt;
    std::time_t updatedAt;

    SumDBRow(int id, int chunkStart, int chunkEnd, std::string topic, std::string summary, std::time_t createdAt, std::time_t updatedAt)
    {
        this->id = id;
        this->chunkStart = chunkStart;
        this->chunkEnd = chunkEnd;
        this->topic = topic;
        this->summary = summary;
        this->createdAt = createdAt;
        this->updatedAt = updatedAt;
    }
}

/*
This function reads all available database names from the input file.
Then it returns a vector of strings containing the names of the databases.
*/
std::vector<std::string> getAllDBNames(const string &topicFileName)
{
    std::vector<std::string> topics;
    ifstream topicFile(topicFileName);
    if (!topicFile)
    {
        cerr << "Error: " << topicFileName << " could not be opened" << endl;
        exit(1);
    }

    std::vector<std::string> containers;
    std::string line;
    while (std::getline(topicFile, line))
    {
        containers.push_back(line);
    }

    topicFile.close();

    return containers;
}

/*
Get all data from 1 database by chunk
*/
struct TopicNodeRow getDBDataByChunk(const std::string &dbName, const std::string &chunkSize)
{
    struct TopicNodeRow data;
    // TODO: To be implemented

    return data;
}

/*
Fetch data through SumAI to get chunk summary
*/
std::string getChunkSummary(const vector<TopicNodeRow> &data)
{
    // TODO: To be implemented
    // Convert data to string
    // Call SumAI to get chunk summary
    return "";
}

/*
Store chunk summary to SumDB
*/
void storeChunkSummary(const std::string &sumdbName, const std::string &summary, const std::string &topic, const std::vector<TopicNodeRow> &data)
{
    // TODO: To be implemented
    // Extract necessary information from data
    int chunkStart = data[0].id;
    int chunkEnd = data[data.size() - 1].id;

    std::time_t now = std::time(0);

    // Create SumDBRow
    SumDBRow sumDBRow(0, chunkStart, chunkEnd, topic, summary, now, now);

    // Init a connection to SumDB
    std::string host = "sumdb";
    std::string port = "5432";
    std::string username = "username";
    std::string password = "password";
    std::string dbname = "db"; // internal database name

    
    // Store SumDBRow to SumDB
}
