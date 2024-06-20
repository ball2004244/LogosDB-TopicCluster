#include "keyword_counter.hpp"

/*
This function aggregates the keywords from the data

It find all keywords, then return a keywords frequency map,
Each keyword is mapped to a List of rowId containing that keyword
*/

std::unordered_map<std::string, std::vector<int>> keywordAggregate(const std::vector<TopicNodeRow> &data)
{
    if (data.empty())
        return {};

    if (data[0].keywords.empty())
        return {}; // No keywords to aggregate

    std::unordered_map<std::string, std::vector<int>> freq;
    for (const auto &row : data)
    {
        int id = row.id;
        for (const auto &word : row.keywords)
            freq[word].push_back(id);
    }
    return freq;
}

/*
This function converts the summary to a string to store in SumDB
*/
std::string convertSummaryToString(const std::unordered_map<std::string, std::vector<int>> &summary)
{
    std::string result = "{";
    for (const auto &pair : summary)
    {
        result += "'" + pair.first + "': [";
        for (const auto &id : pair.second)
        {
            result += std::to_string(id) + ",";
        }
        if (!pair.second.empty())
        {
            result.back() = ']'; // Replace the last comma with a closing bracket
        }
        else
        {
            result += "]";
        }
        result += ", ";
    }
    if (!summary.empty())
    {
        result = result.substr(0, result.length() - 2); // Remove the last comma and space
    }
    result += "}";
    return result;
}