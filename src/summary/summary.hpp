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
#include <future> // Ensure python finished before releasing GIL
#include "schema.hpp"
#include "database/database.hpp"

// #include "ai_summary.hpp" //! Approach 1: Summarize using GenAI (deprecated)
// #include "keyword_counter.hpp" //! Approach 2: Summarize using keyword counter (statistical)
#include "extract_summary.hpp" //! Approach 3: Summarize using extractive summarization



std::vector<std::string> split(const std::string &s, char delimiter);
std::time_t parseTime(const std::string& timeStr);
std::string escape(const std::string &s);
void reformatChunk(pqxx::result &result, std::vector<TopicNodeRow> &data);
void storeChunkSummary(const std::string &summary, const std::string &topic, const std::vector<TopicNodeRow> &data);

#endif // SUMMARY_H