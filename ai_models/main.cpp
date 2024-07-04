#include <cpr/cpr.h>
#include <iostream>
#include <string>

int main() {
    std::string host = "http://localhost:8000";
    std::string endpoint = "/keywords";
    std::string url = host + endpoint;

    // JSON payload
    std::string input = "Your input text here";
    std::string jsonPayload = R"({"text": ")" + input + R"("})";
    // Make a POST request
    auto response = cpr::Post(cpr::Url{url},
                              cpr::Header{{"Content-Type", "application/json"}},
                              cpr::Body{jsonPayload});

    // Check if the request was successful
    if (response.status_code == 200) {
        // Process the response text (assuming it's JSON, XML, or plain text)
        std::cout << "Response from FastAPI server: " << response.text << std::endl;
    } else {
        // Handle errors or unsuccessful requests
        std::cout << "Failed to get a successful response. Status code: " << response.status_code << std::endl;
    }

    return 0;
}