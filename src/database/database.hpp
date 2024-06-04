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

class TopicCluster
{
public:
    TopicCluster(const std::string &dbname, const std::string &username, const std::string &password, const std::string &host, const std::string &port);
    ~TopicCluster();
    void executeQuery(const std::string &sql);
    pxxx::result executeQueryWithResult(const std::string &sql);

private:
    std::unique_ptr<pqxx::connection> connection;
};

// TODO: Rename TopicCluster class to PostgresDB
// TODO: Create TopicCluster class by extending PostgresDB
// TODO: Define clustering mechanism instead of connection to 1 db in TopicCluster
// TODO: Move topics read from build_compose.cpp to TopicCluster class

// TODO: Create SumDB class by extending PostgresDB
#endif // DATABASE_H