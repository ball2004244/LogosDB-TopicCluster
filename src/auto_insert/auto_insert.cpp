#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "database/database.hpp"

/*
This file accept data from csv and insert it to db cluster
The data will be routed to the corresponding topic node
*/
// A helper function to convert a keywords string into postgres array of text
std::string toPostgresArray(const std::string& s)
{
    std::string result = "{";
    size_t start = 0;
    size_t end = s.find(",");
    while (end != std::string::npos)
    {
        result += s.substr(start, end - start) + ",";
        start = end + 1;
        end = s.find(",", start);
    }
    result += s.substr(start, end - start) + "}";
    return result;
}

// TODO: This functions replace original string with some weird chars, need to fix
// A helper function to escape char before insert to db
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

// A helper function to safely insert data to db
std::string createSafePostgresString(const std::string& field) {
    // Base delimiter
    std::string delimiter = "$Q$";
    std::string modifiedDelimiter = delimiter;

    // Check if the delimiter is in the field and modify it if necessary
    int counter = 0;
    while (field.find(modifiedDelimiter) != std::string::npos) {
        modifiedDelimiter = "$Q" + std::to_string(++counter) + "$";
    }

    // Return the field enclosed in the safe delimiter
    return modifiedDelimiter + field + modifiedDelimiter;
}

// A helper function to insert data in batch
template <typename T>
void insertBatchData(std::string table, std::vector<std::string> columns, std::vector<std::vector<T>> data, TopicCluster &cluster)
{
    std::string query = "INSERT INTO " + table + " (";
    for (const auto &column : columns)
    {
        query += column + ", ";
    }
    query = query.substr(0, query.size() - 2) + ") VALUES ";

    for (const auto &row : data)
    {
        query += "(";

        for (const auto &field : row)
            query += createSafePostgresString(field) + ", ";

        query = query.substr(0, query.size() - 2) + "), ";
    }

    query = query.substr(0, query.size() - 2) + ";";

    // Print first 1k characters of the query
    // std::cout << "Executing query: " << query.substr(0, 1000) << std::endl;
    cluster.executeQuery(query);
}

// A helper function to insert data to 1 specific topic node
void insertData(std::string topic, std::string port, std::string username, std::string password, std::string table, std::vector<std::vector<std::string>> data, TopicCluster &cluster, int CHUNK_SIZE = 10000) {
    resetTopicNode(); // Prevent concurrent access to the same topic node
    std::cout << "Inserting data to topic node: " << topic << std::endl;
    cluster.setTopicNode(topic, port, username, password);

    if (data.empty())
    {
        throw std::invalid_argument("No data to insert");
        return;
    }

    // Create a table
    // Hardcoded columns for now
    std::vector<std::string> columns = {"question", "answer", "keywords", "updatedAt"};
    std::string query = "CREATE TABLE IF NOT EXISTS " 
                        + table
                        + " (id SERIAL PRIMARY KEY, "
                        + columns[0] + " TEXT, "
                        + columns[1] + " TEXT, "
                        + columns[2] + " TEXT[], "
                        + columns[3] + " TIMESTAMP);";

    cluster.executeQuery(query);

    // Insert data in batches
    for (int i = 0; i < data.size(); i += CHUNK_SIZE)
    {
        std::cout << "Inserting batch " << i / CHUNK_SIZE + 1 << " of " << (data.size() + CHUNK_SIZE - 1) / CHUNK_SIZE << "(Batch size: " << CHUNK_SIZE << ")" << std::endl;
        int endIdx = std::min(i + CHUNK_SIZE, (int)data.size());

        std::vector<std::vector<std::string>> batch;
        for (int j = i; j < endIdx; j++)
        {
            // Convert keywords string to PostgreSQL array
            data[j][2] = toPostgresArray(data[j][2]);
            batch.push_back(data[j]);
        }

        insertBatchData<std::string>(table, columns, batch, cluster);

        std::cout << "Batch " << i / CHUNK_SIZE + 1 << " inserted successfully" << std::endl;
    }
    cluster.resetTopicNode();
}
// TODO: Replace this function with the actual classification model
//  For now, we use the column "topic" as the classification result
//* TopicIdx will be unessecary in the future
std::string classifyInput(std::vector<std::string> datum, int topicIdx)
{
    if (datum.empty())
    {
        throw std::invalid_argument("No data to classify");
    }

    if (topicIdx < 0 || topicIdx >= datum.size())
    {
        throw std::invalid_argument("Invalid topic index");
    }

    return datum[topicIdx];
}

// TODO: Replace this with the actual keyword extraction model
// For now, return a list of 5 keywords [kw1, kw2, kw3, kw4, kw5]
std::string getKeywords(std::vector<std::string> data)
{
    if (data.empty())
    {
        throw std::invalid_argument("No data to extract keywords");
    }

    return "kw1,kw2,kw3,kw4,kw5";
}

// After classification, we need to reformat the data
// The data will be sorted based on the topic
std::map<std::string, std::vector<std::vector<std::string>>> reformatData(std::vector<std::vector<std::string>> data)
{
    std::cout << "Reformatting data" << std::endl;
    if (data.empty())
    {
        throw std::invalid_argument("No data to reformat");
    }

    // Create a map to store the data by topic
    std::map<std::string, std::vector<std::vector<std::string>>> dataByTopic;

    // Populate the map
    for (auto &row : data)
    {
        int topicIdx = 2; // Assume topic is in 3rd column
        std::string topic = classifyInput(row, topicIdx);
        if (topic.empty())
            throw std::invalid_argument("Empty topic from classification");
    
        if (dataByTopic.find(topic) == dataByTopic.end())
            dataByTopic[topic] = std::vector<std::vector<std::string>>();

        // For each row, first remove topic column
        row.erase(row.begin() + topicIdx);

        // Then add 2 more columns for keywords and current time
        std::string keywords = getKeywords(row);
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm* tm = std::localtime(&now);
        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S"); // Format time as "YYYY-MM-DD HH:MI:SS"
        std::string currentTime = ss.str();

        // Add keywords and current time to the row
        row.insert(row.end(), {keywords, currentTime});
        dataByTopic[topic].push_back(row);

        // Clean up memory
        std::vector<std::string>().swap(row);
    }

    // Now the data is sorted based on the topic
    std::cout << "Data reformatting completed" << std::endl;
    return dataByTopic;
}

int main()
{
    try
    {
        std::cout << "Starting auto insert interface" << std::endl;
        std::string filename = "inputs/input.csv";
        std::string topicFileName = "inputs/topics.txt";
        CSVParser parser(filename);
        std::string port = "5432";
        std::string username = "user";
        std::string password = "password";
        std::string table = "test"; // Assume the table name is test
        TopicCluster cluster(topicFileName);

        // Read and process the data in chunks
        std::vector<std::vector<std::string>> chunk;
        int CHUNK_SIZE = 10000; // Adjust this value based on your system's memory capacity
        while (parser.readChunk(chunk, CHUNK_SIZE))
        {
            // Reformat the data
            std::map<std::string, std::vector<std::vector<std::string>>> reformattedData = reformatData(chunk);

            // This will loop through all the topics and insert the data to the corresponding topic node
            for (const auto &[topic, data] : reformattedData) {
                // Insert the data into the database
                insertData(topic, port, username, password, table, data, cluster, CHUNK_SIZE);
            }
            chunk.clear(); // Clear the chunk to free up memory
        }

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}