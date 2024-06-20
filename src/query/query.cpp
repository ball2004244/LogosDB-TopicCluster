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
        std::string topicFileName = "inputs/topics.txt";

        std::string topic = "Automotive"; // the topic of the node, use as hostname when calling within docker network
        std::string port = "5432";
        std::string dbname = "db"; // internal database name
        std::string username = "user";
        std::string password = "password";

        TopicCluster cluster(topicFileName);
        cluster.setTopicNode(topic, port, username, password, dbname);

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