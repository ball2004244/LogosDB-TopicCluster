from keybert import KeyBERT
import pybind11

'''
pip install keybert pybind11

Then run c++ code with:
g++ -O3 -Wall `python3 -m pybind11 --includes` main.cpp -o main `python3-config --ldflags`
'''

def extract_keywords(text: str) -> list:
    kw_model = KeyBERT()
    keywords = kw_model.extract_keywords(text, keyphrase_ngram_range=(1, 2), stop_words=None)
    return keywords

# Expose the function to C++ using PyBind11
def create_bindings(m):
    m.def("extract_keywords", &extract_keywords, "Extract keywords from text");

PYBIND11_MODULE(kw_extract_module, m) {
    create_bindings(m);
}
