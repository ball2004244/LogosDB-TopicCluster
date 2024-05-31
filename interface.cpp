#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <getopt.h>
#include <fstream>
#include <sstream>

/*
This file serve as the sql interface between the program and the postgres database cluster.
It first locate the requested node, then execute the corresponding sql query to the database.
*/

// Struct for the long options
static struct option long_options[] = {
    {"topic", required_argument, 0, 't'},
    {"port", required_argument, 0, 'p'},
    {"dbname", required_argument, 0, 'd'},
    {"user", required_argument, 0, 'u'},
    {"password", required_argument, 0, 'w'},
    {0, 0, 0, 0}};

// Function to process the input arguments
void processArguments(int argc, char *argv[], std::string &topic, std::string &port, std::string &dbname, std::string &username, std::string &password)
{
    // Default values
    topic = "db1"; // the topic of the node, use as hostname when calling within docker network
    port = "5432";
    dbname = "db"; // internal database name
    username = "user";
    password = "password";

    // Process the arguments
    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "t:p:d:u:w:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 't':
            topic = optarg;
            break;
        case 'p':
            port = optarg;
            break;
        case 'd':
            dbname = optarg;
            break;
        case 'u':
            username = optarg;
            break;
        case 'w':
            password = optarg;
            break;
        default:
            break;
        }
    }
}

// Function to establish a database connection
std::unique_ptr<pqxx::connection> establishConnection(const std::string &topic, const std::string &port, const std::string &dbname, const std::string &username, const std::string &password)
{
    std::string connection_string = "dbname = " + dbname + " user = " + username + " password = " + password + " host = " + topic + " port = " + port;

    // Introduce a connection here
    auto C = std::make_unique<pqxx::connection>(connection_string);

    if (!C->is_open())
    {
        std::cerr << "Can't connect to database: " << dbname << std::endl;
        throw std::runtime_error("Failed to establish database connection");
    }

    std::cout << "Successfully connected to database: " << dbname << std::endl;

    return C;
}

// Function to execute SQL query
void executeQuery(pqxx::connection &connection)
{
    // Create a transactional object
    pqxx::work W(connection);

    // Read SQL query from file
    std::string sqlFile = "query.sql";
    std::ifstream file(sqlFile);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sql = buffer.str();

    // Execute SQL query
    pqxx::result R = W.exec(sql);

    // Print the result
    std::cout << "Operation done successfully" << std::endl;

    // Print whatever the query returns
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

int main(int argc, char *argv[])
{
    try
    {
        std::string topic, port, dbname, username, password;

        // Process the input arguments
        processArguments(argc, argv, topic, port, dbname, username, password);

        // Establish a database connection
        auto connection = establishConnection(topic, port, dbname, username, password);

        // Execute SQL query
        executeQuery(*connection);

        // Disconnect from the database
        connection->disconnect();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl; // print the error message
        return 1;
    }
}