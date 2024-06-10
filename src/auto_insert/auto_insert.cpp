#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "database/database.hpp"

/*
This file accept data from csv and insert it to db cluster
The data will be routed to the corresponding topic node
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

// A helper function to insert data in batch
void insertBatchData(std::string table, std::vector<std::string> columns, std::vector<std::vector<std::string>> data, TopicCluster &cluster)
{
    // Reformat the data to be inserted
    std::string query = "INSERT INTO " + table + " ("; // Create the query
    for (const auto &column : columns)
    {
        query += column + ", ";
    }
    query = query.substr(0, query.size() - 2) + ") VALUES ";

    // Insert data in batches
    for (const auto &row : data)
    {
        query += "(";

        for (const auto &field : row)
        {
            query += "'" + escape(field) + "', ";
        }

        // Remove the last comma and space
        query = query.substr(0, query.size() - 2);

        query += "), ";
    }

    // Remove the last comma and space
    query = query.substr(0, query.size() - 2) + ";";

    // Execute query
    cluster.executeQuery(query);
}

// A helper function to insert data to 1 specific topic node
void insertData(std::string topic, std::string port, std::string username, std::string password, std::string table, std::vector<std::vector<std::string>> data, TopicCluster &cluster, std::vector<std::string> columns)
{
    int BATCH_SIZE = 10000;
    std::cout << "Inserting data to topic node: " << topic << std::endl;
    cluster.setTopicNode(topic, port, username, password);

    if (data.empty())
    {
        throw std::invalid_argument("No data to insert");
        return;
    }

    // Create a table
    std::string query = "CREATE TABLE IF NOT EXISTS " + table + " (";
    for (const auto &column : columns)
    {
        query += column + " TEXT, "; // Assume all columns are of type TEXT
    }
    query = query.substr(0, query.size() - 2) + ");";

    // Execute query
    cluster.executeQuery(query);

    // Insert data in batches
    for (int i = 0; i < data.size(); i += BATCH_SIZE)
    {
        std::cout << "Inserting batch " << i / BATCH_SIZE + 1 << " of " << (data.size() + BATCH_SIZE - 1) / BATCH_SIZE << "(Batch size: " << BATCH_SIZE << ")" << std::endl;
        int endIdx = std::min(i + BATCH_SIZE, (int)data.size());

        std::vector<std::vector<std::string>> batch;
        for (int j = i; j < endIdx; j++)
            batch.push_back(data[j]);

        insertBatchData(table, columns, batch, cluster);

        // Clear the batch
        std::vector<std::vector<std::string>>().swap(batch);

        std::cout << "Batch " << i / BATCH_SIZE + 1 << " inserted successfully" << std::endl;
    }
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

// After classification, we need to reformat the data
// The data will be sorted based on the topic
std::map<std::string, std::vector<std::vector<std::string>>> reformatData(std::vector<std::vector<std::string>> data)
{
    std::cout << "Reformatting data" << std::endl;
    if (data.empty())
    {
        throw std::invalid_argument("No data to reformat");
    }

    // Find the topic col idx by extract header row
    std::vector<std::string> header = data[0];
    int topicIdx = -1;

    for (int i = 0; i < header.size(); i++)
    {
        if (header[i] != "topic")
            continue;

        topicIdx = i;
        break;
    }

    if (topicIdx == -1)
    {
        throw std::invalid_argument("Topic column not found");
    }

    // After that, delete the topic column
    data.erase(data.begin());

    // Create a map to store the data by topic
    std::map<std::string, std::vector<std::vector<std::string>>> dataByTopic;

    // Populate the map
    for (const auto &row : data)
    {
        std::string topic = classifyInput(row, topicIdx);

        if (topic.empty())
            throw std::invalid_argument("Empty topic from classification");

        if (dataByTopic.find(topic) == dataByTopic.end())
            dataByTopic[topic] = std::vector<std::vector<std::string>>();
        dataByTopic[topic].push_back(row);
    }

    // Now the data is sorted based on the topic
    std::cout << "Data reformatting completed" << std::endl;
    return dataByTopic;
}


//! This function is not working properly
int main()
{
    try
    {
        std::cout << "Starting auto insert interface" << std::endl;
        // First we need to get the data from the csv file
        std::string filename = "inputs/input.csv";
        std::string topicFileName = "inputs/topics.txt";
        CSVParser parser(filename);
        std::vector<std::vector<std::string>> data = parser.getData();
        std::vector<std::string> columns = data[0];

        // Then we need to classify the data
        std::cout << "Classifying data" << std::endl;
        std::map<std::string, std::vector<std::vector<std::string>>> dataByTopic = reformatData(data);

        // Reclaim mem for data
        std::cout << "Reclaiming memory after reformatting" << std::endl;
        std::vector<std::vector<std::string>>().swap(data);

        std::string port = "5432";
        std::string username = "user";
        std::string password = "password";
        std::string table = "test"; // Assume the table name is test
        TopicCluster cluster(topicFileName);
        
        // Now loop through the data and insert it to the database by topic
        std::cout << "Start Inserting data to cluster" << std::endl;
        for (const auto &[topic, data] : dataByTopic)
            insertData(topic, port, username, password, table, data, cluster, columns);
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}