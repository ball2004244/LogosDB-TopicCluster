#include "summary.hpp"

/*
This file get all data from Postgres Cluster by chunk, fetch data through SumAI, then use results to build SumDB.
*/

/*
Helper function to split string by delimiter

*/
std::vector<std::string> split(const std::string &s, char delimiter)
{
    if (s.empty())
    {
        return std::vector<std::string>();
    }

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
A helper function to safely cast field to type T

*/

template<typename T>
T safe_as(const pqxx::field& field, T default_value) {
    if (field.is_null()) {
        return default_value;
    }
    try {
        return field.as<T>();
    } catch (const std::exception&) {
        return default_value;
    }
}

/*
Reformat all data from 1 database by chunk
*/
void reformatChunk(pqxx::result &result, std::vector<TopicNodeRow> &data) {
    for (const auto &row : result) {
        int id = safe_as<int>(row[0], 0);
        std::string question = safe_as<std::string>(row[1], "");
        std::string answer = safe_as<std::string>(row[2], "");
        std::string keywordsStr = safe_as<std::string>(row[3], "");
        std::vector<std::string> keywords = {};
        if (!keywordsStr.empty()) keywords = split(keywordsStr, ',');
        std::time_t updatedAt = safe_as<std::time_t>(row[4], 0); // Assuming parseTime can handle the default value appropriately

        TopicNodeRow topicNodeRow(id, question, answer, keywords, updatedAt);
        data.push_back(topicNodeRow);
    }
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
        long long CHUNK_SIZE = 10000;

        //! This method is deprecated, use keywordCounter instead
        // CURL *curl = curl_easy_init();

        // if (!curl)
        //     throw std::runtime_error("Failed to initialize curl");

        std::string query = "";
        std::string table = "test"; // Assume the table name is test for all db in cluster
        long long table_size = 0;
        long long margin = 3 * CHUNK_SIZE; // Set margin error to 3 chunks, prevent from missing data
        long long chunk_count = 0;
        long long order = 0;
        int count = 0;
        // Loop through all topics
        for (auto &topic : topics)
        {
            // Init a connection
            std::cout << "Current topic order: " << ++count << "/" << topics.size() << std::endl;
            cluster.setTopicNode(topic, port, username, password);
            
            // Check if a table exists
            query = "SELECT to_regclass('" + table + "');";
            if (cluster.executeQueryWithResult(query)[0][0].is_null())
            {
                std::cerr << "Table " << table << " does not exist in db: " << topic << std::endl;
                continue;
            }

            if (cluster.executeQueryWithResult(query)[0][0].as<std::string>().empty())
            {
                std::cerr << "Table " << table << " does not exist in db: " << topic << std::endl;
                continue;
            }

            //! MUST replace this with a more efficient way to get table size for big data
            query = "SELECT COUNT(*) FROM " + table + ";";

            // Get estimate table size
            table_size = cluster.executeQueryWithResult(query)[0][0].as<long long>();
            chunk_count = table_size / CHUNK_SIZE + 1;
            order = 0;
            std::cout << "Table size: " << table_size << std::endl;
            std::cout << "Chunk count: " << chunk_count << std::endl;
            // Now query by chunk in each topic node, deallocate memory after each chunk
            for (long long i = 0; i < table_size; i += CHUNK_SIZE)
            {
                std::cout << "Start chunk " << ++order << "/" << chunk_count << std::endl;
                std::vector<TopicNodeRow> data;
                // Get data by chunk
                query = "SELECT * FROM " + table + " LIMIT " + std::to_string(CHUNK_SIZE) + " OFFSET " + std::to_string(i) + ";";

                pqxx::result result = cluster.executeQueryWithResult(query);

                // Reformat data to TopicNodeRow
                reformatChunk(result, data);

                // Get chunk summary
                //! This method is deprecated, use keywordCounter instead
                // std::string summary = getChunkSummary(data, curl, CHUNK_SIZE);

                std::string summary = convertSummaryToString(keywordAggregate(data));


                // TODO: fix bug here, the sumID has gap when doing insert
                //! Assumtion: Gap arises on concurrent write to the same table, so we need to lock the table
                // Store chunk summary to SumDB
                storeChunkSummary(summary, topic, data);

                // Wait for 5 seconds before processing next chunk
                std::cout << "Sleep for 5 seconds" << std::endl;
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