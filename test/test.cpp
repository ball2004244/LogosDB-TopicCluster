#include <iostream>
#include <string>
#include <vector>
#include "database/database.hpp"

int main() {
    try {
        // This file will be used to test the database class
        std::string topicFile = "topics.txt";
        TopicCluster cluster(topicFile);
        std::vector<std::string> topics = cluster.getTopics();

        std::cout << "TOPICS:" << std::endl;
        for (auto &topic : topics) {
            std::cout << topic << std::endl;
        }

        std::cout << "TOPICS COUNT: " << topics.size() << std::endl;

        // Test the setTopicNode method
        std::string port = "5432";
        std::string username = "user";
        std::string password = "password";
        std::string dbname = "db"; // internal db inside container
        std::string table = "test"; // Assume the table name is test for all db in cluster

        // Test on first ele of topics
        std::string topic = topics[0];

        std::cout << "TEST CONNECT TO TOPIC: " << topic << std::endl;
        cluster.setTopicNode(topic, port, username, password, dbname);
        
        std::cout << "TEST execQuery:" << std::endl;
        cluster.executeQuery("SELECT * FROM " + table + " LIMIT 100;");

        std::cout << "TEST execQueryWithResult:" << std::endl;
        pqxx::result res = cluster.executeQueryWithResult("SELECT * FROM " + table + " LIMIT 1;");

        // this will print the first row of the result
        std::cout << res[0][0].as<std::string>() << std::endl; 
        
        std::cout << "TEST DISCONNECT FROM TOPIC: " << topic << std::endl;
        cluster.resetTopicNode();

        topic = topics[1];
        std::cout << "TEST ON NEW TOPIC: " << topic << std::endl;
        // cluster.setTopicNode(topic, port, username, password, dbname);
        std::cout << "Do nothing for now" << std::endl;
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}