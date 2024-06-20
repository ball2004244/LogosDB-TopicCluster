#include "ai_summary.hpp"

/*
A helper function to write callback function for curl
*/
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

/*
Fetch data through SumAI to get chunk summary
*/

std::string getChunkSummary(const std::vector<TopicNodeRow> &data, CURL *curl, long long CHUNK_SIZE)
{
    // Convert data to string
    std::string dataStr = "";
    for (const auto &row : data)
    {
        std::string keywordsStr = "";
        for (const auto &keyword : row.keywords)
        {
            keywordsStr += keyword + " ";
        }
        dataStr += std::to_string(row.id) + " " + row.answer + " " + row.question + " " + keywordsStr + " " + std::to_string(row.updatedAt) + "\n";
    }

    // Build summary prompt
    std::string prompt = std::string("Summarize the following data by bullets like this:\n") +
                         "- [70 % ] Data 1\n" +
                         "- [20 % ] Data 2,\n" +
                         "...\n" +
                         "Below are " + std::to_string(CHUNK_SIZE) + " data needed to summarize :\n " +
                         dataStr;

    // Set up Gemini Service, load api from env
    const char *api = std::getenv("GEMINI_API_KEY");
    const std::string model = "gemini-1.0-pro";

    if (api == NULL)
    {
        std::cerr << "API key not found" << std::endl;
        return "";
    }

    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + model + ":generateContent?key=" + std::string(api);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set the headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set the POST data
    Json::Value root;
    Json::Value contents;
    Json::Value parts;
    parts["text"] = prompt;
    contents["parts"].append(parts);
    root["contents"].append(contents);
    Json::StreamWriterBuilder writer;
    std::string postData = Json::writeString(writer, root);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

    // Set the callback function
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        curl_slist_free_all(headers); // Free the list of headers
        throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
    }

    // Parse the response
    Json::CharReaderBuilder reader;
    Json::Value jsonResponse;
    std::string errors;
    std::istringstream iss(response);
    if (!Json::parseFromStream(reader, iss, &jsonResponse, &errors))
    {
        curl_slist_free_all(headers); // Free the list of headers
        throw std::runtime_error("Failed to parse JSON response: " + errors);
    }

    if (!jsonResponse.isMember("candidates"))
    {
        curl_slist_free_all(headers); // Free the list of headers
        throw std::runtime_error("JSON response does not contain 'candidates' field");
    }

    const Json::Value &candidates = jsonResponse["candidates"];
    if (candidates.empty() || !candidates[0].isMember("content") || !candidates[0]["content"].isMember("parts"))
    {
        curl_slist_free_all(headers); // Free the list of headers
        throw std::runtime_error("JSON response does not contain 'content' or 'parts' field");
    }

    const Json::Value &partsResponse = candidates[0]["content"]["parts"];
    if (partsResponse.empty() || !partsResponse[0].isMember("text"))
    {
        curl_slist_free_all(headers); // Free the list of headers
        throw std::runtime_error("JSON response does not contain 'text' field");
    }

    std::string text = partsResponse[0]["text"].asString();

    // Cleanup
    curl_slist_free_all(headers); // Free the list of headers

    return text;
}