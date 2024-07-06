#include "kw_extract.hpp"

// Callback function to capture the response data
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Get keywords from the kw-extract-server
std::string getKeywords(const std::string& inputText) {
    std::string host = "kw-extract-server";
    std::string port = "8000";
    std::string endpoint = "/keywords";
    std::string url = "http://" + host + ":" + port + endpoint;

    
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::string postData = "{\"text\":\"" + inputText + "\"}";

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L); // Let libcurl determine the size
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up callback function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        // Check for errors
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Always cleanup
        curl_easy_cleanup(curl);
        // Free the custom headers
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();

    return readBuffer;
}


std::map<std::string, float> getKeywordsMap(const std::string& inputText) {
    std::string response = getKeywords(inputText);
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(response, root);
    if (!parsingSuccessful) {
        std::cerr << "Failed to parse the JSON response\n";
        return {};
    }

    std::map<std::string, float> keywordsMap;
    for (const auto& keyword : root) {
        keywordsMap[keyword[0].asString()] = keyword[1].asFloat();
    }

    return keywordsMap;
}

std::string reformatKeywords(const std::map<std::string, float>& keywordsMap) {
    std::string reformattedKeywords;
    for (const auto& [keyword, score] : keywordsMap) {
        reformattedKeywords += keyword + ",";
    }
    reformattedKeywords.pop_back(); // Remove the trailing comma
    return reformattedKeywords;
}