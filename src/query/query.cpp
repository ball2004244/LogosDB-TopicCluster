#include <iostream>
#include <string>
#include "database/database.hpp"

/*
This file query data from a specific table
*/

int main()
{
    try
    {
        std::string filename = "inputs/input.csv";

        std::string topic = "db1"; // the topic of the node, use as hostname when calling within docker network
        std::string port = "5432";
        std::string dbname = "db"; // internal database name
        std::string username = "user";
        std::string password = "password";

        TopicCluster cluster(dbname, username, password, topic, port);

        std::string table = "test"; // Name of table to query
        std::string query = "SELECT * FROM " + table + ";";

        // Execute the query
        cluster.executeQuery(query);

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}