#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>

namespace py = pybind11;


// TODO: Need to fix pybind header error
int main() {
    Py_Initialize(); // Initialize the Python interpreter
    auto kw_extract_module = py::module_::import("kw_extract_module");
    auto extract_keywords = kw_extract_module.attr("extract_keywords");

    std::string text = "Your text to extract keywords from";
    auto keywords = extract_keywords(text).cast<std::vector<std::string>>();

    for (const auto& keyword : keywords) {
        std::cout << keyword << std::endl;
    }

    Py_Finalize(); // Finalize the Python interpreter
    return 0;
}