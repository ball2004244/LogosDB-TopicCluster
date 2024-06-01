#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <pqxx/pqxx>
#include <memory>

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

private:
    std::unique_ptr<pqxx::connection> connection;
};

#endif // DATABASE_H