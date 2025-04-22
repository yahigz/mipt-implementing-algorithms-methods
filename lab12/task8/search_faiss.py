from langchain.embeddings import HuggingFaceEmbeddings
from langchain.vectorstores import FAISS
from langchain.docstore.document import Document

import re

with open('text.txt', 'r') as f:
    text = f.read()

sentences = re.split(r'(?<!\w\.\w.)(?<![A-ZА-Я][a-zа-я]\.)(?<=\.|\?|\!)\s', text)
sentences = [s.strip() for s in sentences if s.strip()]

docs = [Document(page_content=sentence) for sentence in sentences]

embeddings = HuggingFaceEmbeddings(model_name="sentence-transformers/paraphrase-multilingual-MiniLM-L12-v2")

db = FAISS.from_documents(docs, embeddings)

with open('query.txt', 'r') as f:
    query = f.read().strip()

similar_docs = db.similarity_search_with_score(query, k=5)

print(f"Запрос: '{query}'\n")
for i, (doc, score) in enumerate(similar_docs):
    print(f"{i+1}. {doc.page_content} (сходство: {score:.4f})")