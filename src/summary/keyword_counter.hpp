#ifndef KEYWORD_COUNTER_HPP
#define KEYWORD_COUNTER_HPP

#include <unordered_map>
#include <vector>
#include <string>
#include "schema.hpp"

std::unordered_map<std::string, std::vector<int>> keywordAggregate(const std::vector<TopicNodeRow> &data);
std::string convertSummaryToString(const std::unordered_map<std::string, std::vector<int>> &summary);

#endif // KEYWORD_COUNTER_HPP