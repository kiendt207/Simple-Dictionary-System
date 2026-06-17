# 🌳 Tried-based Simple Dictionary System

![Language](https://img.shields.io/badge/Language-C-blue.svg)
![Data Structures](https://img.shields.io/badge/Data%20Structures-Prefix%20Tree%20(Trie)%20%7C%20Queue-orange.svg)
![Algorithms](https://img.shields.io/badge/Algorithms-BFS%20%7C%20DFS%20%7C%20Backtracking-success.svg)

A simple, in-memory dictionary and Natural Language Processing (NLP) engine built entirely in C. This project utilizes a Trie (Prefix Tree) data structure to deliver lightning-fast text processing features, including spell checking, frequency-based autocomplete, and wildcard pattern matching.

## 🚀 Key Features

* **Spell Checker:** Analyzes text files using a custom character-by-character parser. It intelligently sanitizes input by ignoring digits and punctuation, verifying words against a loaded standard English dictionary (`words.txt`), and logging misspelled words.
* **Smart Autocomplete (BFS):** Implements a Breadth-First Search (BFS) algorithm with a custom Queue to suggest word completions. Results are automatically sorted in descending order based on historical usage frequency.
* **Wildcard Search (DFS):** Employs Depth-First Search (DFS) with backtracking to resolve pattern-matching queries (e.g., `c_t` returns `cat`, `cot`, `cut`), dynamically navigating valid Trie branches.
* **Complete CRUD User Dictionary:** Users have full control over their personal vocabulary. The system exposes an interactive menu allowing users to insert new words and **delete existing words** seamlessly.
* **State Persistence (Serialization):** Converts the complex pointer-based Trie network into a flattened CSV format (`personal_dict.csv`) using DFS, allowing the user's custom dictionary and usage frequencies to be safely saved to the hard drive and restored upon the next launch.

## 🧠 Core Algorithms & Complexity

| Operation / Feature | Underlying Algorithm | Time Complexity |
| :--- | :--- | :--- |
| **Insert / Search Word** | Iterative Trie Traversal | O(L) |
| **Delete Word** | Post-order DFS (Soft delete & Pruning) | O(L) |
| **Autocomplete** | BFS via Array-based Queue + QuickSort | O(V + E) + O(k log k) |
| **Wildcard Matching** | DFS with Backtracking | O(A^L) (Worst case) |
| **Garbage Collection** | Post-order DFS | O(N) |

*(Note: `L` is the length of the word, `A` is the alphabet size (256), `V` and `E` are vertices and edges in the Trie branch, `k` is the number of suggestions, and `N` is the total number of nodes).*

## 🛠️ Build and Run Instructions

**Prerequisites**
* GCC Compiler installed on your system.
* A standard English dictionary file named words.txt placed in the working directory.

## 💻 CLI Interactive Menu

Upon launch, the engine loads the dictionaries into RAM and presents the following dashboard:

* **Spell Checker:** Input a text file. The system filters out numbers/symbols and outputs words missing from the Trie.

* **Autocomplete / Wildcard:** Type a prefix (e.g., app) for frequency-sorted suggestions, or use underscores for wildcard search (e.g., _ppl_).

* **Learn New Word:** Insert a custom word into the in-memory Trie (Updates frequency if it already exists).

* **Save Dictionary:** Serialize the current RAM state to personal_dict.csv.

* **Delete Word:** Remove a word from the Trie. The algorithm carefully unflags the word and prunes orphaned leaf nodes without breaking paths for other words.

* **Exit:** Triggers a strict post-order DFS garbage collection routine to free all allocated memory before safely terminating.