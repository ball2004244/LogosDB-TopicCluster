#ifndef SUMMARY_H
#define SUMMARY_H

#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include "schema.hpp"
#include "database/database.hpp"

// #include "ai_summary.hpp" //! Summarize using AI, currently deprecated
#include "keyword_counter.hpp" // Summarize using keyword counter, a statistical approach



std::vector<std::string> split(const std::string &s, char delimiter);
std::time_t parseTime(const std::string& timeStr);
std::string escape(const std::string &s);
void reformatChunk(pqxx::result &result, std::vector<TopicNodeRow> &data);
void storeChunkSummary(const std::string &summary, const std::string &topic, const std::vector<TopicNodeRow> &data);

#endif // SUMMARY_H