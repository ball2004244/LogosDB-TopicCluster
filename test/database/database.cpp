#include "database.hpp"
/*
This file support interaction with csv file & postgres database
*/

/*
Create a csv parser
*/
CSVParser::CSVParser(std::string filename) : file(filename)
{
    if (!file.is_open())
    {
        std::cout << "Failed to open file\n";
        return;
    }
}

// Get data as a whole
std::vector<std::vector<std::string>> CSVParser::getData()
{
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> row;
        bool inQuotes = false;

        std::ostringstream fieldStream;
        for (char ch : line)
        {
            switch (ch)
            {
                case ',':
                    if (inQuotes)
                    {
                        fieldStream << ch;
                    }
                    else
                    {
                        row.push_back(fieldStream.str());
                        fieldStream.str("");
                        fieldStream.clear();
                    }
                    break;
                case '\"':
                    inQuotes = !inQuotes;
                    break;
                default:
                    fieldStream << ch;
                    break;
            }
        }
        row.push_back(fieldStream.str());
        data.push_back(row);
    }

    return data;
}

const bool CSVParser::isEmpty() const
{
    return data.empty();
}

// Get data by chunk
bool CSVParser::readChunk(std::vector<std::vector<std::string>> &chunk, int chunkSize)
{
    chunk.clear();
    std::string line;
    int linesRead = 0;

    while (linesRead < chunkSize && std::getline(file, line))
    {
        std::istringstream ss(line);
        std::vector<std::string> row;
        bool inQuotes = false;
        std::ostringstream fieldStream;

        for (char ch : line)
        {
            switch (ch)
            {
                case ',':
                    if (inQuotes)
                    {
                        fieldStream << ch;
                    }
                    else
                    {
                        row.push_back(fieldStream.str());
                        fieldStream.str("");
                        fieldStream.clear();
                    }
                    break;
                case '\"':
                    inQuotes = !inQuotes;
                    break;
                default:
                    fieldStream << ch;
                    break;
            }
        }
        row.push_back(fieldStream.str());
        chunk.push_back(row);
        ++linesRead;
    }

    // Return true if any lines were read, indicating there was data.
    return linesRead > 0;
}

/*
Create a connection to the database
*/

PostgresDB::PostgresDB(const std::string &dbname, const std::string &username, const std::string &password, const std::string &host, const std::string &port)
{
    std::string connection_string = "dbname = " + dbname + " user = " + username + " password = " + password + " host = " + host + " port = " + port;

    try {
        // Introduce a connection here
        connection = std::make_unique<pqxx::connection>(connection_string);

        if (!connection->is_open())
        {
            std::cerr << "Can't connect to database: " << dbname << std::endl;
            throw std::runtime_error("Failed to establish database connection");
        }

        std::cout << "Successfully connected to database: " << dbname << std::endl;
    }
    catch (const pqxx::broken_connection &e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
        throw std::runtime_error("Failed to establish database connection");
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void PostgresDB::executeQuery(const std::string &sql, bool verbose)
{
    try
    {
        // Create a transactional object
        pqxx::work W(*connection);

        // Execute SQL query
        pqxx::result R = W.exec(sql);

        // Print the result
        if (verbose)
        {
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
        }

        // Commit the transaction
        W.commit();
    }
    catch (const pqxx::sql_error &e)
    {
        // Output to 1k word of query
        std::string query = e.query();
        std::cout << "Query was: " << query.substr(0, 1000) << std::endl;
        std::cerr << "SQL error: " << e.what() << std::endl;
        throw std::runtime_error("Failed to execute SQL query");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}
// Return the pxqq output after query
pqxx::result PostgresDB::executeQueryWithResult(const std::string &sql)
{
    try
    {
        // Create a transactional object
        pqxx::work W(*connection);

        // Execute SQL query
        pqxx::result R = W.exec(sql);

        // Commit the transaction
        W.commit();

        return R;
    }
    catch (const pqxx::sql_error &e)
    {
        std::string query = e.query();
        std::cout << "Query was: " << query.substr(0, 1000) << std::endl;
        std::cerr << "SQL error: " << e.what() << std::endl;
        throw std::runtime_error("Failed to execute SQL query");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void PostgresDB::disconnect() {
    if (connection->is_open()) {
        connection->disconnect();
        std::cout << "Disconnected from database" << std::endl;
    }
}

bool PostgresDB::isConnected() const {
    return connection && connection->is_open();
}

PostgresDB::~PostgresDB()
{
    disconnect();
}

TopicCluster::TopicCluster(const std::string &topicFileName)
{
    std::ifstream file(topicFileName);

    if (!file)
    {
        std::cerr << "Error: " << topicFileName << " could not be opened" << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line))
        topics.push_back(line);
}

TopicCluster::~TopicCluster()
{
    std::cout << "Topic Cluster terminated" << std::endl;
}

void TopicCluster::setTopicNode(const std::string &topic, const std::string &port, const std::string &username, const std::string &password, const std::string &dbname)
{
    // Disconnect previous connection if it exists
    std::cout << "Setting topic node: " << topic << std::endl;
    if (connection != nullptr) {
        std::cout << "Found existing connection, disconnecting..." << std::endl;
        connection->disconnect();
    }

    // Initialize new connection
    connection = std::make_unique<PostgresDB>(dbname, username, password, topic, port);
    std::cout << "New connection established" << std::endl;
}

void TopicCluster::resetTopicNode()
{
    if (connection == nullptr)
    {
        std::cerr << "No connection to reset" << std::endl;
        return;
    }

        std::cout << "Found existing connection, disconnecting..." << std::endl;
        connection->disconnect();
        if (!connection->isConnected()) {
            std::cout << "Successfully disconnected." << std::endl;
        } else {
            std::cerr << "Disconnection failed." << std::endl;
        }
        connection = nullptr;

    std::cout << "Disconnected any existing connection" << std::endl;
}

std::vector<std::string> TopicCluster::getTopics()
{
    return topics;
}

void TopicCluster::executeQuery(const std::string &sql)
{
    if (connection == nullptr)
    {
        std::cerr << "Error: No connection to database" << std::endl;
        return;
    }

    connection->executeQuery(sql);
}

pqxx::result TopicCluster::executeQueryWithResult(const std::string &sql)
{
    if (connection == nullptr)
    {
        std::cerr << "Error: No connection to database" << std::endl;
        throw std::runtime_error("No connection to database");
    }

    return connection->executeQueryWithResult(sql);
}

SumDB::SumDB(const std::string &dbname, const std::string &username, const std::string &password, const std::string &host, const std::string &port,const std::string &tableName) : PostgresDB(dbname, username, password, host, port)
{
    std::string query = "CREATE TABLE IF NOT EXISTS " + tableName + " (id SERIAL PRIMARY KEY, chunkStart INT, chunkEnd INT, topic TEXT, summary TEXT, updatedAt TIMESTAMP);";
    executeQuery(query);

    std::cout << "SumDB object created" << std::endl;
}

SumDB::~SumDB()
{
    std::cout << "SumDB object destroyed" << std::endl;
}