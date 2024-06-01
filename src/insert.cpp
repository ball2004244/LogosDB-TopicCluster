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
        CSVParser parser(filename);

        std::string topic = "db1"; // the topic of the node, use as hostname when calling within docker network
        std::string port = "5432";
        std::string dbname = "db"; // internal database name
        std::string username = "user";
        std::string password = "password";

        TopicCluster cluster(dbname, username, password, topic, port);

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
        // Reformat the data to be inserted
        std::string query = "INSERT INTO " + table + " (";
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

        // Check by querying the data
        query = "SELECT * FROM " + table + ";";
        cluster.executeQuery(query);

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}