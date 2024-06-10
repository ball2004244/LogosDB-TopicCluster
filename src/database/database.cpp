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

void PostgresDB::executeQuery(const std::string &sql)
{
    try
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
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << std::endl;
        std::cerr << "Query was: " << e.query() << std::endl;
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
        std::cerr << "SQL error: " << e.what() << std::endl;
        std::cerr << "Query was: " << e.query() << std::endl;
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

PostgresDB::~PostgresDB()
{
    disconnect();
}

TopicCluster::TopicCluster(const std::string &topicFileName)
{
    // Read topics from file
    CSVParser parser(topicFileName);

    if (parser.isEmpty())
    {
        std::cerr << "Error: " << topicFileName << " could not be opened" << std::endl;
        exit(1);
    }

    topics = parser.getData()[0];
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

SumDB::SumDB(const std::string &dbname, const std::string &username, const std::string &password, const std::string &host, const std::string &port) : PostgresDB(dbname, username, password, host, port)
{
    std::string query = "CREATE TABLE IF NOT EXISTS " + dbname + " (id SERIAL PRIMARY KEY, chunkStart INT, chunkEnd INT, topic TEXT, summary TEXT, createdAt TIMESTAMP, updatedAt TIMESTAMP);";
    executeQuery(query);

    std::cout << "SumDB object created" << std::endl;
}

SumDB::~SumDB()
{
    std::cout << "SumDB object destroyed" << std::endl;
}