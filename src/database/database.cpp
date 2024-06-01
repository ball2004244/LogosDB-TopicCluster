#include "database.hpp"
/*
This file support interaction with csv file & postgres database
*/

/*
Create a csv parser
*/
CSVParser::CSVParser(std::string filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cout << "Failed to open file\n";
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> row;


        while (std::getline(ss, field, ','))
        {
            // Remove newline character
            field.erase(std::remove(field.begin(), field.end(), '\n'), field.end());
            row.push_back(field);
        }


        data.push_back(row);
    }
}

const std::vector<std::vector<std::string>> &CSVParser::getData() const
{
    return data;
}

const bool CSVParser::isEmpty() const
{
    return data.empty();
}

/*
Create a connection to the database cluster
*/

TopicCluster::TopicCluster(const std::string &dbname, const std::string &username, const std::string &password, const std::string &host, const std::string &port)

{
    std::string connection_string = "dbname = " + dbname + " user = " + username + " password = " + password + " host = " + host + " port = " + port;

    // Introduce a connection here
    connection = std::make_unique<pqxx::connection>(connection_string);

    if (!connection->is_open())
    {
        std::cerr << "Can't connect to database: " << dbname << std::endl;
        throw std::runtime_error("Failed to establish database connection");
    }

    std::cout << "Successfully connected to database: " << dbname << std::endl;
}

void TopicCluster::executeQuery(const std::string &sql)
{
    // Create a transactional object
    pqxx::work W(*connection);

    // Execute SQL query
    pqxx::result R = W.exec(sql);

    // Print the result
    std::cout << "Operation done successfully" << std::endl;

    // Print whatever the query returns
    std::cout << "Query result: " << std::endl;
    for (auto row : R)
    {
        for (auto field : row)
        {
            std::cout << field.as<std::string>() << ' ';
        }
        std::cout << std::endl;
    }

    // Commit the transaction
    W.commit();
}

TopicCluster::~TopicCluster()
{
    connection->disconnect();
    std::cout << "Disconnected from database" << std::endl;
}