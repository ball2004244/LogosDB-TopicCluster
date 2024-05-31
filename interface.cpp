#include <pqxx/pqxx>
#include <iostream>
#include <string>

/*
This file serve as the sql interface between the program and the postgres database cluster.
It first locate the requested node, then execute the corresponding sql query to the database.
*/

int main()
{
    try
    {
        std::string network = "db_network"; // the name of the docker network

        std::string topic = "db1"; // the topic of the node, use as hostname when calling within docker network
        std::string port = "5432";

        std::string dbname = "db"; // internal database name
        std::string username = "user";
        std::string password = "password";

        std::string connection_string = "dbname = " + dbname + " user = " + username + " password = " + password + " host = " + topic + " port = " + port;

        // Introduce a connection here
        pqxx::connection C(connection_string);

        if (!C.is_open())
        {
            std::cerr << "Can't connect to database: " << dbname << std::endl;
            return 1;
        }

        std::cout << "Opened database successfully: " << dbname << std::endl;

        // Create a transactional object
        // pqxx::work W(C);

        // // Execute SQL query
        // std::string sql = "SELECT * FROM COMPANY";
        // pqxx::result R = W.exec(sql);

        // // Print the result
        // std::cout << "Operation done successfully" << std::endl;

        // for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        // {
        //     std::cout << "ID = " << c[0].as<int>() << std::endl;
        //     std::cout << "NAME = " << c[1].as<std::string>() << std::endl;
        //     std::cout << "AGE = " << c[2].as<int>() << std::endl;
        //     std::cout << "ADDRESS = " << c[3].as<std::string>() << std::endl;
        //     std::cout << "SALARY = " << c[4].as<float>() << std::endl;
        // }

        // Commit the transaction
        // W.commit();
        C.disconnect();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl; // print the error message
        return 1;
    }
}