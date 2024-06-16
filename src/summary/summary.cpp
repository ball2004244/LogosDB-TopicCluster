#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstddef>
#include <curl/curl.h>
#include <json/json.h>
#include <thread>
#include <chrono>
#include <iomanip>
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
    std::string question;
    std::string answer;
    std::vector<std::string> keywords;
    std::time_t updatedAt;

    TopicNodeRow(int id, std::string answer, std::string question, std::vector<std::string> keywords, std::time_t updatedAt)
    {
        this->id = id;
        this->answer = answer;
        this->question = question;
        this->keywords = keywords;
        this->updatedAt = updatedAt;
    }
};

struct SumDBRow
{
    int id;
    int chunkStart;
    int chunkEnd;
    std::string topic;
    std::string summary;
    std::time_t updatedAt;

    SumDBRow(int id, int chunkStart, int chunkEnd, std::string topic, std::string summary, std::time_t updatedAt)
    {
        this->id = id;
        this->chunkStart = chunkStart;
        this->chunkEnd = chunkEnd;
        this->topic = topic;
        this->summary = summary;
        this->updatedAt = updatedAt;
    }
};

/*
A helper function to write callback function for curl
*/
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

/*
Helper function to split string by delimiter

*/
std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

/*
Helper function to parse datetime
*/

std::time_t parseTime(const std::string& timeStr) {
    std::tm tm = {};
    std::stringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return std::mktime(&tm);
}

/*
Hlper function to escape char
*/
//A helper function to escape char before insert to db
std::string escape(const std::string &s)
{
    std::string result;
    for (char c : s)
    {
        switch (c)
        {
            case '\'': result += "''"; break; // Escape single quote with another single quote
            case '\"': result += "\\\""; break; // Escape double quote
            case '\\': result += "\\\\"; break; // Escape backslash
            default: result += c;
        }
    }
    return result;
}

/*
Reformat all data from 1 database by chunk
*/
void reformatChunk(pqxx::result &result, std::vector<TopicNodeRow> &data)
{
    for (const auto &row : result)
    {
        int id = row[0].as<int>();
        std::string question = row[1].as<std::string>();
        std::string answer = row[2].as<std::string>();
        std::string keywordsStr = row[3].as<std::string>();
        std::vector<std::string> keywords = split(keywordsStr, ',');

        std::time_t updatedAt = parseTime(row[4].as<std::string>());

        TopicNodeRow topicNodeRow(id, question, answer, keywords, updatedAt);
        data.push_back(topicNodeRow);
    }
}
/*
Fetch data through SumAI to get chunk summary
*/

std::string getChunkSummary(const std::vector<TopicNodeRow> &data, CURL *curl, long long CHUNK_SIZE)
{
    // Convert data to string
    std::string dataStr = "";
    for (const auto &row : data)
    {
        std::string keywordsStr = "";
        for (const auto &keyword : row.keywords)
        {
            keywordsStr += keyword + " ";
        }
        dataStr += std::to_string(row.id) + " " + row.answer + " " + row.question + " " + keywordsStr + " " + std::to_string(row.updatedAt) + "\n";
    }

    // Build summary prompt
    std::string prompt = std::string("Summarize the following data by bullets like this:\n") +
                         "- [70 % ] Data 1\n" +
                         "- [20 % ] Data 2,\n" +
                         "...\n" +
                         "Below are " + std::to_string(CHUNK_SIZE) + " data needed to summarize :\n " +
                         dataStr;

    // Set up Gemini Service, load api from env
    const char *api = std::getenv("GEMINI_API_KEY");
    const std::string model = "gemini-1.0-pro";

    if (api == NULL)
    {
        std::cerr << "API key not found" << std::endl;
        return "";
    }

    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + model + ":generateContent?key=" + std::string(api);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set the headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set the POST data
    Json::Value root;
    Json::Value contents;
    Json::Value parts;
    parts["text"] = prompt;
    contents["parts"].append(parts);
    root["contents"].append(contents);
    Json::StreamWriterBuilder writer;
    std::string postData = Json::writeString(writer, root);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

    // Set the callback function
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        curl_slist_free_all(headers); // Free the list of headers
        throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
    }

    // Parse the response
    Json::CharReaderBuilder reader;
    Json::Value jsonResponse;
    std::string errors;
    std::istringstream iss(response);
    if (!Json::parseFromStream(reader, iss, &jsonResponse, &errors))
    {
        curl_slist_free_all(headers); // Free the list of headers
        throw std::runtime_error("Failed to parse JSON response: " + errors);
    }

    if (!jsonResponse.isMember("candidates"))
    {
        curl_slist_free_all(headers); // Free the list of headers
        throw std::runtime_error("JSON response does not contain 'candidates' field");
    }

    const Json::Value &candidates = jsonResponse["candidates"];
    if (candidates.empty() || !candidates[0].isMember("content") || !candidates[0]["content"].isMember("parts"))
    {
        curl_slist_free_all(headers); // Free the list of headers
        throw std::runtime_error("JSON response does not contain 'content' or 'parts' field");
    }

    const Json::Value &partsResponse = candidates[0]["content"]["parts"];
    if (partsResponse.empty() || !partsResponse[0].isMember("text"))
    {
        curl_slist_free_all(headers); // Free the list of headers
        throw std::runtime_error("JSON response does not contain 'text' field");
    }

    std::string text = partsResponse[0]["text"].asString();

    // Cleanup
    curl_slist_free_all(headers); // Free the list of headers

    return text;
}

/*
Store chunk summary to SumDB
*/
void storeChunkSummary(const std::string &summary, const std::string &topic, const std::vector<TopicNodeRow> &data)
{
    // Extract necessary information
    int chunkStart = data[0].id;
    int chunkEnd = data[data.size() - 1].id;

    std::time_t now = std::time(0);

    // Create SumDBRow
    SumDBRow sumDBRow(0, chunkStart, chunkEnd, topic, summary, now);

    // Init a connection to SumDB
    std::string host = "logosdb-sumdb";
    std::string port = "5432";
    std::string username = "user";
    std::string password = "password";
    std::string dbname = "db"; // internal database name
    std::string sumTable = "test"; // Assume the table name is test for all db in cluster

    // Store SumDBRow to SumDB
    SumDB sumdb(dbname, username, password, host, port, sumTable);
    std::string query = "INSERT INTO " + sumTable + " (chunkStart, chunkEnd, topic, summary, updatedAt) \
                        VALUES (" +
                        std::to_string(sumDBRow.chunkStart) + ", " + std::to_string(sumDBRow.chunkEnd) + ", '" + sumDBRow.topic + "', '" + escape(sumDBRow.summary) + "', TO_TIMESTAMP(" + std::to_string(sumDBRow.updatedAt) + "));";

    sumdb.executeQuery(query);
}

//! Need to test this code snippet to ensure it works as expected
int main()
{
    try
    {
        // Get all topics from TopicCluster
        std::string topicFileName = "inputs/topics.txt";
        TopicCluster cluster(topicFileName);
        std::vector<std::string> topics = cluster.getTopics();

        std::string port = "5432";
        std::string username = "user";
        std::string password = "password";
        long long CHUNK_SIZE = 10;
        CURL *curl = curl_easy_init();

        if (!curl)
            throw std::runtime_error("Failed to initialize curl");

        std::string query = "";
        std::string table = "test"; // Assume the table name is test for all db in cluster
        long long table_size = 0;
        long long margin = 3 * CHUNK_SIZE; // Set margin error to 3 chunks, prevent from missing data
        // Loop through all topics
        for (auto &topic : topics)
        {
            // Init a connection
            cluster.setTopicNode(topic, port, username, password);
            
            query = "SELECT n_live_tup \
                    FROM pg_stat_user_tables \
                    WHERE relname = '" + table + "' AND schemaname = 'public';";

            // Get estimate table size
            table_size = cluster.executeQueryWithResult(query)[0][0].as<long long>();

            // Now query by chunk in each topic node, deallocate memory after each chunk
            for (long long i = 0; i < table_size; i += CHUNK_SIZE)
            {
                std::vector<TopicNodeRow> data;
                // Get data by chunk
                query = "SELECT * FROM " + table + " LIMIT " + std::to_string(CHUNK_SIZE) + " OFFSET " + std::to_string(i) + ";";

                pqxx::result result = cluster.executeQueryWithResult(query);

                // Reformat data to TopicNodeRow
                reformatChunk(result, data);

                // Get chunk summary
                std::string summary = getChunkSummary(data, curl, CHUNK_SIZE);
                // Store chunk summary to SumDB
                storeChunkSummary(summary, topic, data);

                // Wait for 2 seconds before processing next chunk
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}