#include "kw_extract.hpp"

// Function to extract keywords from input text
std::string extractKeywords(const std::string& input) {
    std::string host = "localhost";
    std::string port = "8000";
    std::string endpoint = "/keywords";
    std::string url = "http://" + host + ":" + port + endpoint;

    std::string jsonPayload = R"({"text": ")" + input + R"("})";

    // Make a POST request
    auto response = cpr::Post(cpr::Url{url},
                              cpr::Header{{"Content-Type", "application/json"}},
                              cpr::Body{jsonPayload});

    // Check if the request was successful
    if (response.status_code == 200) {
        // Parse the response text as JSON
        Json::Value jsonResponse;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(response.text, jsonResponse); // Parse the JSON response
        if (!parsingSuccessful) {
            std::cerr << "Failed to parse JSON response" << std::endl;
            return ""; // Or handle the error as appropriate
        }

        // Assuming the response contains a field named "result" with the keywords
        if (jsonResponse.isMember("result")) {
            return jsonResponse["result"].asString();
        } else {
            std::cerr << "JSON response does not contain 'result' field" << std::endl;
            return "";
        }
    } else {
        // Handle errors or unsuccessful requests
        std::cerr << "Failed to get a successful response. Status code: " << response.status_code << std::endl;
        return "";
    }
}
