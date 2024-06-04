#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <pqxx/pqxx>
#include <memory>
#include <algorithm>

class CSVParser
{
public:
    CSVParser(std::string filename);
    const std::vector<std::vector<std::string>> &getData() const;
    const bool isEmpty() const;

private:
    std::vector<std::vector<std::string>> data;
};

class PostgresDB
{
public:
    PostgresDB(const std::string &dbname, const std::string &username, const std::string &password, const std::string &host, const std::string &port);
    ~PostgresDB();
    void executeQuery(const std::string &sql);
    pqxx::result executeQueryWithResult(const std::string &sql);

protected:
    std::unique_ptr<pqxx::connection> connection;
};

// TopicCluster manage multiple topics
class TopicCluster
{
public:
    TopicCluster(const std::string &topicFileName);
    ~TopicCluster();
    void setTopicNode(const std::string &topic, const std::string &port, const std::string &username, const std::string &password, const std::string &dbname = "db");
    std::vector<std::string> getTopics();
    void executeQuery(const std::string &sql);
    pqxx::result executeQueryWithResult(const std::string &sql);

private:
    std::unique_ptr<PostgresDB> connection;
    std::vector<std::string> topics;
};

// Create SumDB class by extending PostgresDB
class SumDB : public PostgresDB
{
public:
    SumDB(const std::string &dbname, const std::string &username, const std::string &password, const std::string &host, const std::string &port);
    ~SumDB();
};
#endif // DATABASE_H