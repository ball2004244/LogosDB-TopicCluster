from typing import List, Tuple
from keybert import KeyBERT
from fastapi import FastAPI
from pydantic import BaseModel
import json
app = FastAPI()

class TextRequest(BaseModel):
    text: str

@app.post("/keywords")
def extract_keywords(request: TextRequest) -> List[Tuple[str, float]]:
    print(f'User request: {request.text}')
    kw_model = KeyBERT()
    keywords = kw_model.extract_keywords(request.text, keyphrase_ngram_range=(1, 2), stop_words=None)
    return keywords

# pip install fastapi uvicorn keybert
# uvicorn kw_extract:app --host 0.0.0.0 --port 8000