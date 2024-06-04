#include <iostream>
#include <string>
#include "database/database.hpp"

/*
This file insert data from csv to db cluster
*/

int main()
{
    try
    {
        std::string filename = "inputs/input.csv";
        std::string topicFileName = "inputs/topics.txt";
        CSVParser parser(filename);

        std::string topic = "db1"; // the topic of the node, use as hostname when calling within docker network
        std::string port = "5432";
        std::string dbname = "db"; // internal database name
        std::string username = "user";
        std::string password = "password";

        TopicCluster cluster(topicFileName);
        cluster.setTopicNode(topic, port, username, password, dbname);

        std::vector<std::vector<std::string>> data = parser.getData();
        std::string table = "test";

        if (data.empty())
        {
            std::cout << "No data to insert" << std::endl;
            return 0;
        }

        // Extract the title row to be used as column names
        std::vector<std::string> columns = data[0];

        data.erase(data.begin()); // This will remove the title row from the data

        // Create a table for first data
        std::string query = "CREATE TABLE IF NOT EXISTS " + table + " (";
        for (const auto &column : columns)
        {
            query += column + " TEXT, "; // Assume all columns are of type TEXT
        }
        query = query.substr(0, query.size() - 2) + ");";

        // Execute query
        cluster.executeQuery(query);

        // Reformat the data to be inserted
        query = "INSERT INTO " + table + " (";
        for (const auto &column : columns)
        {
            query += column + ", ";
        }
        query = query.substr(0, query.size() - 2) + ") VALUES ";

        // Reformat the data to be inserted
        for (const auto &row : data)
        {
            query += "(";

            for (const auto &field : row)
            {
                query += "'" + field + "', ";
            }

            // Remove the last comma and space
            query = query.substr(0, query.size() - 2);

            query += "), ";
        }

        // Remove the last comma and space
        query = query.substr(0, query.size() - 2) + ";";

        // Execute query
        cluster.executeQuery(query);

        std::cout << "Data inserted successfully" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}