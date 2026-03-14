#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
using namespace std;

// Tokenizer
vector<string> tokenize(string str) {
    vector<string> tokens;
    string token;
    stringstream ss(str);
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// LCS (Dynamic Programming)
int longestCommonSubsequence(vector<string>& a, vector<string>& b) {
    int m = a.size(), n = b.size();
    vector<vector<int>> dp(m+1, vector<int>(n+1, 0));
    for (int i = 1; i <= m; ++i)
        for (int j = 1; j <= n; ++j)
            if (a[i-1] == b[j-1])
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
    return dp[m][n];
}

// N-Grams
vector<string> getNGrams(vector<string>& tokens, int n) {
    vector<string> ngrams;
    if (tokens.size() < n) return ngrams;
    for (size_t i = 0; i <= tokens.size() - n; ++i) {
        string ngram = "";
        for (int j = 0; j < n; ++j) {
            ngram += tokens[i + j];
            if (j < n - 1) ngram += " ";
        }
        ngrams.push_back(ngram);
    }
    return ngrams;
}

// Merge Sort for N-Grams
void merge(vector<string>& arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    vector<string> L(arr.begin() + l, arr.begin() + m + 1);
    vector<string> R(arr.begin() + m + 1, arr.begin() + r + 1);
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<string>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l)/2;
        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);
        merge(arr, l, m, r);
    }
}

// Compare N-Grams
vector<string> matchedNGrams(vector<string>& base, vector<string>& user) {
    unordered_set<string> baseSet(base.begin(), base.end());
    vector<string> matches;
    for (const string& gram : user) {
        if (baseSet.find(gram) != baseSet.end()) {
            matches.push_back(gram);
        }
    }
    return matches;
}

// NEW FUNCTIONS BEGIN HERE

// Function to load synonyms data from file - updated for colon delimiter
unordered_map<string, vector<string>> loadSynonyms(const string& filename) {
    unordered_map<string, vector<string>> synonymsMap;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening " << filename << endl;
        return synonymsMap;
    }

    string line;
    while (getline(file, line)) {
        size_t colonPos = line.find(':');
        if (colonPos != string::npos) {
            // Extract the key word (before the colon)
            string word = line.substr(0, colonPos);

            // Remove trailing spaces from the word
            word.erase(word.find_last_not_of(" \t") + 1);

            // Extract the synonyms part (after the colon)
            string synonymsStr = line.substr(colonPos + 1);

            // Parse synonyms
            vector<string> synonymsList;
            istringstream synStream(synonymsStr);
            string synonym;

            while (synStream >> synonym) {
                // Remove comma if it exists
                if (synonym.back() == ',') {
                    synonym.pop_back();
                }
                synonymsList.push_back(synonym);
            }

            synonymsMap[word] = synonymsList;
        }
    }

    file.close();
    return synonymsMap;
}

// Function to clean a word (remove punctuation for matching)
string cleanWord(const string& word) {
    string cleaned = word;
    // Remove common punctuation
    for (char& c : cleaned) {
        if (ispunct(c)) {
            c = ' ';
        }
    }
    // Remove extra spaces
    stringstream ss(cleaned);
    ss >> cleaned;
    return cleaned;
}

// Function to suggest synonyms for words in matched phrases
void suggestSynonymsForMatches(const vector<string>& matches, const unordered_map<string, vector<string>>& synonymsMap) {
    cout << "\n----- Synonym Suggestions for Matched Phrases -----\n";

    for (const string& match : matches) {
        cout << "For phrase: \"" << match << "\"" << endl;

        // Tokenize the matched phrase
        vector<string> words = tokenize(match);
        bool hasSuggestions = false;

        for (const string& word : words) {
            // Clean the word and convert to lowercase for matching
            string cleanedWord = cleanWord(word);
            transform(cleanedWord.begin(), cleanedWord.end(), cleanedWord.begin(), ::tolower);

            // Check if we have synonyms for this word
            auto it = synonymsMap.find(cleanedWord);
            if (it != synonymsMap.end() && !it->second.empty()) {
                hasSuggestions = true;
                cout << "  Instead of \"" << word << "\", consider: ";
                for (size_t i = 0; i < it->second.size(); ++i) {
                    cout << it->second[i];
                    if (i < it->second.size() - 1) {
                        cout << ", ";
                    }
                }
                cout << endl;
            }
        }

        if (!hasSuggestions) {
            cout << "  No synonyms found for words in this phrase." << endl;
        }
        cout << endl;
    }
}

// Function to find words that contribute most to plagiarism
void suggestWordsToChange(const vector<string>& tokensBase, const vector<string>& tokensUser,
                         const unordered_map<string, vector<string>>& synonymsMap) {
    cout << "\n----- Words to Change to Reduce Plagiarism -----\n";

    // Count word frequencies in the base text
    unordered_map<string, int> baseFreq;
    for (const string& word : tokensBase) {
        string cleanedWord = cleanWord(word);
        transform(cleanedWord.begin(), cleanedWord.end(), cleanedWord.begin(), ::tolower);
        if (!cleanedWord.empty()) {
            baseFreq[cleanedWord]++;
        }
    }

    // Count which words from user text also appear in base text
    unordered_map<string, int> commonWords;
    unordered_map<string, string> originalForm; // To keep original casing

    for (const string& word : tokensUser) {
        string cleanedWord = cleanWord(word);
        transform(cleanedWord.begin(), cleanedWord.end(), cleanedWord.begin(), ::tolower);

        if (!cleanedWord.empty() && baseFreq.find(cleanedWord) != baseFreq.end()) {
            commonWords[cleanedWord]++;
            // Keep the original form for display
            if (originalForm.find(cleanedWord) == originalForm.end()) {
                originalForm[cleanedWord] = word;
            }
        }
    }

    // Sort words by frequency (most common first)
    vector<pair<string, int>> sortedCommon(commonWords.begin(), commonWords.end());
    sort(sortedCommon.begin(), sortedCommon.end(),
         [](const pair<string, int>& a, const pair<string, int>& b) {
             return a.second > b.second;
         });

    // Display top words to change (up to 10)
    int count = 0;
    for (const auto& pair : sortedCommon) {
        if (count >= 10) break;

        auto it = synonymsMap.find(pair.first);
        if (it != synonymsMap.end() && !it->second.empty()) {
            string displayWord = originalForm[pair.first];
            cout << "The word \"" << displayWord << "\" appears " << pair.second
                 << " times and could be replaced with: ";

            for (size_t i = 0; i < it->second.size(); ++i) {
                cout << it->second[i];
                if (i < it->second.size() - 1) {
                    cout << ", ";
                }
            }
            cout << endl;
            count++;
        }
    }

    if (count == 0) {
        cout << "No common words with available synonyms found." << endl;
    }
}

// Main
int main() {
    string originalText, userText, line;

    // Read base file
    ifstream baseFile("Dataset.txt");
    if (!baseFile.is_open()) {
        cerr << "Error opening Dataset.txt\n";
        return 1;
    }
    while (getline(baseFile, line)) {
        originalText += line + " ";
    }
    baseFile.close();

    // Take user document input
    cout << "Paste Your content:\n";
    getline(cin, userText);
    ofstream userFile("user_dataset.txt");
    userFile << userText;
    userFile.close();
    cout << "User input saved to user_dataset.txt\n";

    // Get N-Gram size
    int n = 2;
   /* cout << "Enter N-Gram Size: ";
    cin >> n;
    cin.ignore();*/

    // Tokenization
    vector<string> tokensBase = tokenize(originalText);
    vector<string> tokensUser = tokenize(userText);
    if (tokensUser.size() < n || tokensBase.size() < n) {
        cout << "\nNot enough words for the selected N-Gram size.\n";
        return 1;
    }

    // LCS Check
    int lcsLength = longestCommonSubsequence(tokensBase, tokensUser);
    double lcsPercent = (double)lcsLength / tokensUser.size() * 100.0;

    // N-Gram Check
    vector<string> ngramsBase = getNGrams(tokensBase, n);
    vector<string> ngramsUser = getNGrams(tokensUser, n);
    mergeSort(ngramsBase, 0, ngramsBase.size() - 1);
    mergeSort(ngramsUser, 0, ngramsUser.size() - 1);
    vector<string> matches = matchedNGrams(ngramsBase, ngramsUser);
    double ngramPercent = (double)matches.size() / ngramsUser.size() * 100.0;

    // Final Output
    cout << "\n----- Plagiarism Report -----\n";
    cout << "Similarity: " << lcsPercent << "%\n";
//    cout << "N-Gram Similarity: " << ngramPercent << "%\n";

    /*cout << "\nMatched Phrases:\n";
    for (const auto& phrase : matches) {
        cout << "- " << phrase << "\n";
    }*/

    // NEW CODE: Load synonyms and provide suggestions
    unordered_map<string, vector<string>> synonymsMap = loadSynonyms("synomyns_data.txt");

    // Suggest synonyms for matched phrases
    suggestSynonymsForMatches(matches, synonymsMap);

    // Suggest words to change to reduce plagiarism
    suggestWordsToChange(tokensBase, tokensUser, synonymsMap);

    return 0;
}
