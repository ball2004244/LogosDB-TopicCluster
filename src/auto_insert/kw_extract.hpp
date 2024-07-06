#ifndef KW_EXTRACT_HPP
#define KW_EXTRACT_HPP

#include <string>
#include <map>
#include <curl/curl.h>
#include <json/json.h>
#include <iostream>

// Callback function to capture the response data
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp);
std::string getKeywords(const std::string& inputText);
std::map<std::string, float> getKeywordsMap(const std::string& inputText);
std::string reformatKeywords(const std::map<std::string, float>& keywordsMap);

#endif // KW_EXTRACT_HPP