#ifndef AI_SUMMARY_HPP
#define AI_SUMMARY_HPP

#include <curl/curl.h>
#include <json/json.h>
#include <iostream>
#include <vector>
#include <string>
#include "schema.hpp"

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp);

std::string getChunkSummary(const std::vector<TopicNodeRow> &data, CURL *curl, long long CHUNK_SIZE);

#endif // AI_SUMMARY_HPP