#ifndef EXTRACT_SUMMARY_HPP
#define EXTRACT_SUMMARY_HPP

#include <vector>
#include <string>
#include <utility>
#include <stdexcept>
#include <Python.h>
#include "schema.hpp"

// Function declarations
PyObject* initialize_python();
void finalize_python();
std::vector<std::string> mass_extract_summaries(PyObject* pModule, const std::vector<std::string>& inputs);
std::vector<std::pair<int, std::string>> extractSummary(PyObject* pModule, const std::vector<TopicNodeRow> &data);
std::string convertSummaryToString(const std::vector<std::pair<int, std::string>>& summaries);

#endif // EXTRACT_SUMMARY_HPP