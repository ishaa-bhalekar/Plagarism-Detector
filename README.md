# Plagiarism Detector (C++)

A robust command-line tool designed to identify both exact and paraphrased plagiarism between text documents. This project was developed as part of the B.Tech Computer Science (Data Science) curriculum at MPSTME.

## Features

* **Exact Match Detection:** Uses Longest Common Subsequence (LCS) to find the longest string of identical text.
* **Paraphrase Identification:** Implements N-Grams (sliding window) to detect structural similarities even if words are shifted.
* **Synonym Analysis:** Includes a synonym-loading feature to identify cases where words were swapped to bypass basic detection.
* **Optimization:** Uses Merge Sort for efficient data handling and C++ STL (Unordered Maps/Sets) for fast lookups.

## Tech Stack

* **Language:** C++
* **Algorithms:** Longest Common Subsequence (LCS), N-Grams, Merge Sort.
* **Data Structures:** Vectors, Unordered Maps, Sets.

## How It Works

1. **Preprocessing:** Cleans the text (removes punctuation, converts to lowercase).
2. **Analysis:** Runs the LCS and N-Gram algorithms to calculate a similarity percentage.
3. **Reporting:** Generates a summary of matched segments and suggests potential areas of concern.

## Installation and Usage

1. **Clone the repository:**
```bash
git clone https://github.com/ishaa-bhalekar/Plagarism-Detector.git
cd Plagarism-Detector

```


2. **Compile the code:**
```bash
g++ main.cpp -o detector

```


3. **Run the application:**
```bash
./detector

```



## Contributors

* **Ishaa Bhalekar**
* **Arpita Das**

---
