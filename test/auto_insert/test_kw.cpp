#include <curl/curl.h>
#include <json/json.h>
#include <string>
#include <iostream>
#include <map>

// Callback function to capture the response data
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string getKeywords(const std::string& inputText) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::string postData = "{\"text\":\"" + inputText + "\"}";

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8000/keywords");
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

int main() {
    std::string rawInput = 
        "Biochemistry is the study of chemical processes within and relating to living organisms. "
        "It's a broad and complex field that bridges biology and chemistry, exploring how molecules "
        "like proteins, nucleic acids, carbohydrates, and lipids interact to create the phenomena "
        "we recognize as life. At its core, biochemistry seeks to understand how these biomolecules "
        "are synthesized, how they function, and how they work together in intricate metabolic pathways. "
        "The field encompasses everything from the structure and function of individual molecules to "
        "the complex interactions that occur within cells and entire organisms. Key areas of study "
        "include enzyme kinetics, which examines how proteins catalyze chemical reactions; metabolism, "
        "which looks at how organisms transform energy and matter; and molecular genetics, which explores "
        "how genetic information is stored, replicated, and expressed. Biochemistry has far-reaching "
        "applications in medicine, nutrition, and biotechnology. It forms the basis for understanding "
        "diseases at a molecular level, developing new drugs and therapies, and creating biotechnological "
        "solutions for agriculture and industry. The field has given rise to numerous subfields, including "
        "clinical biochemistry, which applies biochemical knowledge to medical diagnoses; structural "
        "biochemistry, which uses techniques like X-ray crystallography to determine the three-dimensional "
        "structures of biomolecules; and systems biochemistry, which attempts to model entire biochemical "
        "networks within cells. As our understanding of biochemistry grows, so too does our ability to "
        "manipulate biological systems, leading to advancements in areas such as genetic engineering, "
        "personalized medicine, and synthetic biology. The ongoing revolution in biochemical techniques, "
        "including high-throughput sequencing, mass spectrometry, and cryo-electron microscopy, continues "
        "to push the boundaries of what we can observe and understand about the molecular basis of life.";

    std::map<std::string, float> keywordsMap = getKeywordsMap(rawInput);

    for (const auto& [keyword, score] : keywordsMap) {
        std::cout << keyword << ": " << score << std::endl;
    }

    std::string reformattedKeywords = reformatKeywords(keywordsMap);
    std::cout << "Reformatted keywords: " << reformattedKeywords << std::endl;

    return 0;
}
