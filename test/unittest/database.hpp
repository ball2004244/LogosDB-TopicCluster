#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
// #include <pqxx/pqxx>

class CSVParser
{
public:
    CSVParser(std::string filename);
    const bool isEmpty() const;
    std::vector<std::vector<std::string>> getData();
    bool readChunk(std::vector<std::vector<std::string>> &chunk, int chunkSize);

private:
    std::vector<std::vector<std::string>> data;
    std::ifstream file;
};

// class PostgresDB
// {
// public:
//     PostgresDB(const std::string &dbname, const std::string &username, const std::string &password, const std::string &host, const std::string &port);
//     ~PostgresDB();
//     void executeQuery(const std::string &sql, bool verbose = false);
//     pqxx::result executeQueryWithResult(const std::string &sql);
//     void disconnect();
//     bool isConnected() const;

// protected:
//     std::unique_ptr<pqxx::connection> connection;
// };

// // TopicCluster manage multiple topics
// class TopicCluster
// {
// public:
//     TopicCluster(const std::string &topicFileName);
//     ~TopicCluster();
//     void setTopicNode(const std::string &topic, const std::string &port, const std::string &username, const std::string &password, const std::string &dbname = "db");
//     void resetTopicNode();
//     std::vector<std::string> getTopics();
//     void executeQuery(const std::string &sql);
//     pqxx::result executeQueryWithResult(const std::string &sql);

// private:
//     std::unique_ptr<PostgresDB> connection;
//     std::vector<std::string> topics;
// };

// // Create SumDB class by extending PostgresDB
// class SumDB : public PostgresDB
// {
// public:
//     SumDB(const std::string &dbname, const std::string &username, const std::string &password, const std::string &host, const std::string &port, const std::string &tableName);
//     ~SumDB();
// };
#endif // DATABASE_H