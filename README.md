# Mini Search Engine Using Searching and Sorting Algorithms

A second-year DSA project implemented with:

- C++17 + CMake
- React + Vite
- Local `.txt` documents (10 documents)
- Inverted Index with exact and potential prefix/partial match retrieval
- Trie autocomplete with instant single-letter suggestions
- TF-IDF + Cosine Similarity relevance ranking modulated by match quality
- Linear, Sentinel, Binary, Fibonacci, Indexed Sequential Search
- Bubble, Selection, Insertion, Merge, Quick, Heap, Counting, Radix, Bucket Sort
- Live Web Search integration (Wikipedia CORS API & external web portals)
- Lightweight HTTP API implemented directly with sockets (no Crow/ASIO)

## 1. Build the C++ backend

Requirements:

- Windows + MinGW/g++
- CMake
- Ninja (recommended)

From `backend/`:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

Run the CLI:

```bash
./build/SearchEngine.exe
```

Run the HTTP server:

```bash
./build/SearchEngine.exe --server
```

The server listens on:

```text
http://127.0.0.1:8080
```

## 2. Run the React frontend

Requirements: Node.js 18+

From `frontend/`:

```bash
npm install
npm run dev
```

Open the Vite URL shown in the terminal.

The frontend expects the C++ backend at `http://127.0.0.1:8080`.

## 3. API

### Search

```text
GET /api/search?q=machine%20learning&search=Binary%20Search&sort=Quick%20Sort
```

### Autocomplete

```text
GET /api/autocomplete?prefix=alg
```

### Rebuild

```text
POST /api/index/rebuild
```

### Stats

```text
GET /api/stats
```

### Index sample

```text
GET /api/index/sample
```

## 4. Architecture

```text
                    Local TXT Documents
                            |
                            v
                    FileManager / Loader
                            |
                            v
                      Tokenizer
                 /          |          \
                v           v           v
        Stop Words      Normalizer    Stemmer
                \          |          /
                         Tokens
                            |
              +-------------+-------------+
              |                           |
              v                           v
        Inverted Index                  Trie
              |
              v
        Candidate Documents
              |
              v
        TF-IDF + Cosine Similarity
              |
              v
       Selected Sorting Algorithm
              |
              v
          JSON Results
              |
              v
           React UI
```

## 5. Important academic point

The project deliberately does not hide the syllabus algorithms behind STL sorting/search functions. The algorithms are implemented in `backend/src/searching/` and `backend/src/sorting/`.

`std::unordered_map` is used as supporting storage for the inverted index, while the actual index traversal, search algorithms, ranking and sorting demonstrations are implemented explicitly.

## 6. Development order

1. File loading
2. Preprocessing
3. Inverted index
4. Trie
5. Searching algorithms
6. TF-IDF
7. Cosine similarity
8. Sorting algorithms
9. HTTP API
10. React UI

## 7. CLI commands

The CLI supports:

```text
search <query>
autocomplete <prefix>
stats
index
rebuild
help
exit
```

Example:

```text
search machine learning
```
