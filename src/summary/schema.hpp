#ifndef SCHEMA_HPP
#define SCHEMA_HPP

#include <string>
#include <vector>
#include <ctime>

struct TopicNodeRow
{
    int id;
    std::string question;
    std::string answer;
    std::vector<std::string> keywords;
    std::time_t updatedAt;

    TopicNodeRow(int id, std::string answer, std::string question, std::vector<std::string> keywords, std::time_t updatedAt)
    {
        this->id = id;
        this->answer = answer;
        this->question = question;
        this->keywords = keywords;
        this->updatedAt = updatedAt;
    }
};

struct SumDBRow
{
    int id;
    int chunkStart;
    int chunkEnd;
    std::string topic;
    std::string summary;
    std::time_t updatedAt;

    SumDBRow(int id, int chunkStart, int chunkEnd, std::string topic, std::string summary, std::time_t updatedAt)
    {
        this->id = id;
        this->chunkStart = chunkStart;
        this->chunkEnd = chunkEnd;
        this->topic = topic;
        this->summary = summary;
        this->updatedAt = updatedAt;
    }
};

#endif // SCHEMA_HPP