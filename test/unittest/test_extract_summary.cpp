#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>
#include <Python.h>
#include "database.hpp"
#include "schema.hpp"

/*
Must have Python 3.11 installed in a Conda environment for this code to work

Install necessary packages:
conda install -c conda-forge python=3.11 nltk networkx numpy scikit-learn

Compile this file with:
g++ -o summarize test_extract_summary.cpp -I/home/tam/miniconda3/envs/logosdb/include/python3.11 -L/home/tam/miniconda3/envs/logosdb/lib -lpython3.11

Run the compiled file with:
LD_LIBRARY_PATH=/home/tam/miniconda3/envs/logosdb/lib ./summarize
*/

/*
Helper function to initialize the Python interpreter and import the Cython module
*/
PyObject* initialize_python() {
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('.')");

    // Import the Cython module
    PyObject* pModule = PyImport_ImportModule("extract_sum_mp");
    if (!pModule) {
        PyErr_Print();
        throw std::runtime_error("Failed to load extract_sum_mp module");
    }

    return pModule;
}

/*
Helper function to finalize the Python interpreter
*/
void finalize_python() {
    Py_Finalize();
}

/*
Helper function to call the mass_extract_summaries function from the Cython module
*/
std::vector<std::string> mass_extract_summaries(PyObject* pModule, const std::vector<std::string>& inputs) {
    std::vector<std::string> summaries;

    // Get the mass_extract_summaries function
    PyObject* pFunc = PyObject_GetAttrString(pModule, "mass_extract_summaries");
    if (!pFunc || !PyCallable_Check(pFunc)) {
        PyErr_Print();
        Py_XDECREF(pModule); // Clean up the module reference
        throw std::runtime_error("Failed to load mass_extract_summaries function");
    }

    // Convert the inputs to a Python list
    PyObject* pInputs = PyList_New(inputs.size());
    for (size_t i = 0; i < inputs.size(); ++i) {
        PyObject* pValue = PyUnicode_FromString(inputs[i].c_str());
        PyList_SetItem(pInputs, i, pValue);
    }

    // Call the mass_extract_summaries function
    PyObject* pResult = PyObject_CallObject(pFunc, PyTuple_Pack(1, pInputs));
    if (!pResult) {
        PyErr_Print();
        Py_XDECREF(pInputs);
        Py_XDECREF(pFunc);
        Py_XDECREF(pModule);
        throw std::runtime_error("Failed to call mass_extract_summaries function");
    }

    // Convert the result to a C++ vector
    for (Py_ssize_t i = 0; i < PyList_Size(pResult); ++i) {
        PyObject* pItem = PyList_GetItem(pResult, i);
        summaries.push_back(PyUnicode_AsUTF8(pItem));
    }

    // Clean up
    Py_XDECREF(pInputs);
    Py_XDECREF(pResult);
    Py_XDECREF(pFunc);

    return summaries;
}

/*
This function does extractive summarization on the data
*/
std::vector<std::pair<int, std::string>> extractSummary(PyObject* pModule, const std::vector<TopicNodeRow> &data) {
    if (data.empty())
        return {};

    // First concat question and answer for every row
    std::vector<std::string> concatQAText;
    for (const auto &row : data) {
        std::string text = row.question + "\n" + row.answer;
        concatQAText.push_back(text);
    }

    // Summarize the text
    std::vector<std::string> summaries;
    try {
        summaries = mass_extract_summaries(pModule, concatQAText);
    } catch (const std::exception &e) {
        throw;
    }

    // Combine the summaries with the row IDs
    std::vector<std::pair<int, std::string>> result;
    for (size_t i = 0; i < data.size(); ++i) {
        result.push_back({data[i].id, summaries[i]});
    }

    return result;
}

/*
This function converts the summary to a string to store in SumDB
*/
std::string convertSummaryToString(const std::vector<std::pair<int, std::string>>& summaries) {
    if (summaries.empty())
        return "No summaries available";

    std::string result = "'";
    for (const auto& summary : summaries) {
        result += "\n" + std::to_string(summary.first) + ". " + summary.second + "\n";
    }
    result += "'";

    return result;
}

int main() {
    std::string filename = "biology.csv";
    CSVParser parser(filename);

    // Get data from the CSV file with getData
    std::vector<std::vector<std::string>> data = parser.getData();
    if (parser.isEmpty()) {
        std::cout << "No data found\n";
        return 1;
    }

    // Now initialize the Python interpreter and import the Cython module
    PyObject* pModule = initialize_python();

    // Convert the data to TopicNodeRow
    std::vector<TopicNodeRow> topicNodeData;
    for (const auto& row : data) {
        int id = std::stoi(row[0]);
        std::string question = row[1];
        std::string answer = row[2];
        std::vector<std::string> keywords;
        std::time_t updatedAt = std::time(nullptr);

        TopicNodeRow topicNodeRow(id, answer, question, keywords, updatedAt);
        topicNodeData.push_back(topicNodeRow);
    }

    // Extract summaries
    std::vector<std::pair<int, std::string>> summaries;
    try {
        summaries = extractSummary(pModule, topicNodeData);
    } catch (const std::exception &e) {
        std::cerr << "Error during summarization: " << e.what() << std::endl;
        finalize_python();
        return 1;
    }

    // Convert the summaries to a string
    std::string summaryString = convertSummaryToString(summaries);
    std::cout << summaryString << "\n";

    // Clean up
    finalize_python();

    // Return success
    return 0;
}