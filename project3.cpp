// Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <utility>
#include <iomanip>
#include <deque>
#include <locale>
#include <unistd.h> // For getcwd

using namespace std;

bool searchOccurred = false;

struct Log {
    long long timestamp;
    string semicolTimestamp;
    string category;
    string messages;
    int entryId;
};

struct masterCompare {
    bool operator()(const Log &log1, const Log &log2) const {
        if (log1.timestamp < log2.timestamp) {
            return true;
        } else if (log1.timestamp > log2.timestamp) {
            return false;
        } else {
            if (log1.category < log2.category) {
                return true;
            } else if (log1.category > log2.category) {
                return false;
            } else {
                return log1.entryId < log2.entryId;
            }
        }
    }
};

string convertToLower(string input) {
    locale loc;
    for (size_t i = 0; i < input.size(); ++i) {
        input[i] = tolower(input[i], loc);
    }
    return input;
}

void timestampSearch(long long timestamp1, long long timestamp2, vector<Log> &masterLog, vector<int> &recentSearch) {
    size_t left = 0;
    size_t right = masterLog.size();
    while (left < right) {
        size_t mid = (left + right) / 2;
        if (timestamp1 <= masterLog[mid].timestamp) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    if (left == masterLog.size()) {
        cout << "Timestamps search: 0 entries found" << '\n';
        return;
    } else {
        while (left < masterLog.size() && masterLog[left].timestamp <= timestamp2) {
            recentSearch.push_back(static_cast<int>(left));
            ++left;
        }
        cout << "Timestamps search: " << recentSearch.size() << " entries found" << '\n';
    }
}

void matchingSearch(long long timestamp, vector<Log> &masterLog, vector<int> &recentSearch) {
    size_t left = 0;
    size_t right = masterLog.size();
    while (left < right) {
        size_t mid = (left + right) / 2;
        if (timestamp <= masterLog[mid].timestamp) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    if (left == masterLog.size()) {
        cout << "Timestamps search: 0 entries found" << '\n';
        return;
    } else {
        while (left < masterLog.size() && masterLog[left].timestamp == timestamp) {
            recentSearch.push_back(static_cast<int>(left));
            ++left;
        }
        cout << "Timestamp search: " << recentSearch.size() << " entries found" << '\n';
    }
}

void categorySearch(string input, unordered_map<string, vector<int>> &categoryMap, vector<int> &recentSearch) {
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    auto iter = categoryMap.find(input);
    if (iter == categoryMap.end()) {
        cout << "Category search: 0 entries found" << '\n';
    } else {
        for (int index : categoryMap[input]) {
            recentSearch.push_back(index);
        }
        cout << "Category search: " << categoryMap[input].size() << " entries found" << '\n';
    }
}

void keywordSearch(unordered_map<string, vector<int>> &messageMap, vector<string> &keywords, vector<int> &recentSearch) {
    vector<int> intersect;
    for (size_t i = 0; i < keywords.size(); ++i) {
        auto iter = messageMap.find(keywords[i]);
        if (iter == messageMap.end()) {
            cout << "Keyword search: 0 entries found" << '\n';
            return;
        } else {
            if (intersect.empty() && i == 0) {
                intersect = messageMap[keywords[i]];
            } else {
                vector<int> temp;
                auto first1 = intersect.begin();
                auto first2 = messageMap[keywords[i]].begin();
                while (first1 != intersect.end() && first2 != messageMap[keywords[i]].end()) {
                    if (*first1 < *first2) {
                        ++first1;
                    } else if (*first1 > *first2) {
                        ++first2;
                    } else {
                        temp.push_back(*first1);
                        ++first1;
                        ++first2;
                    }
                }
                intersect = temp;
            }
        }
    }
    cout << "Keyword search: " << intersect.size() << " entries found" << '\n';
    for (int index : intersect) {
        recentSearch.push_back(index);
    }
}

void makeKeywordMap(Log &input, unordered_map<string, vector<int>> &messageMap, int index) {
    string catTemp, mesTemp;
    for (size_t i = 0; i < input.category.size(); ++i) {
        if (input.category[i] == ' ' || !isalnum(static_cast<unsigned char>(input.category[i]))) {
            if (!catTemp.empty()) {
                auto iter = messageMap.find(catTemp);
                if (iter == messageMap.end()) {
                    messageMap[catTemp].push_back(index);
                } else {
                    if (!binary_search(messageMap[catTemp].begin(), messageMap[catTemp].end(), index)) {
                        messageMap[catTemp].push_back(index);
                    }
                }
            }
            catTemp.clear();
        } else {
            catTemp += static_cast<char>(tolower(static_cast<unsigned char>(input.category[i])));
            if (i == input.category.size() - 1) {
                auto iter = messageMap.find(catTemp);
                if (iter == messageMap.end()) {
                    messageMap[catTemp].push_back(index);
                } else {
                    if (!binary_search(messageMap[catTemp].begin(), messageMap[catTemp].end(), index)) {
                        messageMap[catTemp].push_back(index);
                    }
                }
            }
        }
    }
    for (size_t i = 0; i < input.messages.size(); ++i) {
        if (input.messages[i] == ' ' || !isalnum(static_cast<unsigned char>(input.messages[i]))) {
            if (!mesTemp.empty()) {
                auto iter = messageMap.find(mesTemp);
                if (iter == messageMap.end()) {
                    messageMap[mesTemp].push_back(index);
                } else {
                    if (!binary_search(messageMap[mesTemp].begin(), messageMap[mesTemp].end(), index)) {
                        messageMap[mesTemp].push_back(index);
                    }
                }
                mesTemp.clear();
            }
        } else {
            mesTemp += static_cast<char>(tolower(static_cast<unsigned char>(input.messages[i])));
            if (i == input.messages.size() - 1) {
                auto iter = messageMap.find(mesTemp);
                if (iter == messageMap.end()) {
                    messageMap[mesTemp].push_back(index);
                } else {
                    if (!binary_search(messageMap[mesTemp].begin(), messageMap[mesTemp].end(), index)) {
                        messageMap[mesTemp].push_back(index);
                    }
                }
            }
        }
    }
}

void appendLog(int entryId, vector<Log> &masterLog, deque<int> &excerptList) {
    for (size_t i = 0; i < masterLog.size(); ++i) {
        if (entryId == masterLog[i].entryId) {
            excerptList.push_back(static_cast<int>(i));
            break;
        }
    }
    cout << "log entry " << entryId << " appended" << '\n';
}

void appendSearch(vector<int> &recentSearch, deque<int> &excerptList) {
    for (int index : recentSearch) {
        excerptList.push_back(index);
    }
    cout << recentSearch.size() << " log entries appended" << '\n';
}

void deleteLog(size_t input, deque<int> &excerptList) {
    if (input < excerptList.size()) {
        excerptList.erase(excerptList.begin() + static_cast<deque<int>::difference_type>(input));
        cout << "Deleted excerpt list entry " << input << '\n';
    } else {
        cerr << "out of bounds input" << '\n';
    }
}

void moveToBegin(size_t input, deque<int> &excerptList) {
    if (input < excerptList.size()) {
        auto it = excerptList.begin() + static_cast<deque<int>::difference_type>(input);
        rotate(excerptList.begin(), it, it + 1);
        cout << "Moved excerpt list entry " << input << '\n';
    } else {
        cerr << "out of bounds input" << '\n';
    }
}

void moveToEnd(size_t input, deque<int> &excerptList) {
    if (input < excerptList.size()) {
        auto it = excerptList.begin() + static_cast<deque<int>::difference_type>(input);
        rotate(it, it + 1, excerptList.end());
        cout << "Moved excerpt list entry " << input << '\n';
    } else {
        cerr << "out of bounds input" << '\n';
    }
}

void sortExcerpt(deque<int> &excerptList, vector<Log> &masterLog) {
    if (excerptList.empty()) {
        cout << "excerpt list sorted" << '\n';
        cout << "(previously empty)" << '\n';
    } else {
        size_t size = excerptList.size() - 1;
        cout << "excerpt list sorted" << '\n';
        cout << "previous ordering:" << '\n';
        cout << 0 << '|' << masterLog[static_cast<size_t>(excerptList[0])].entryId << '|' << masterLog[static_cast<size_t>(excerptList[0])].semicolTimestamp << '|' << masterLog[static_cast<size_t>(excerptList[0])].category << '|' << masterLog[static_cast<size_t>(excerptList[0])].messages << '\n';
        cout << "..." << '\n';
        cout << size << '|' << masterLog[static_cast<size_t>(excerptList[size])].entryId << '|' << masterLog[static_cast<size_t>(excerptList[size])].semicolTimestamp << '|' << masterLog[static_cast<size_t>(excerptList[size])].category << '|' << masterLog[static_cast<size_t>(excerptList[size])].messages << '\n';
        sort(excerptList.begin(), excerptList.end());
        cout << "new ordering:" << '\n';
        cout << 0 << '|' << masterLog[static_cast<size_t>(excerptList[0])].entryId << '|' << masterLog[static_cast<size_t>(excerptList[0])].semicolTimestamp << '|' << masterLog[static_cast<size_t>(excerptList[0])].category << '|' << masterLog[static_cast<size_t>(excerptList[0])].messages << '\n';
        cout << "..." << '\n';
        cout << size << '|' << masterLog[static_cast<size_t>(excerptList[size])].entryId << '|' << masterLog[static_cast<size_t>(excerptList[size])].semicolTimestamp << '|' << masterLog[static_cast<size_t>(excerptList[size])].category << '|' << masterLog[static_cast<size_t>(excerptList[size])].messages << '\n';
    }
}

void clearExcerpt(deque<int> &excerptList, vector<Log> &masterLog) {
    if (excerptList.empty()) {
        cout << "excerpt list cleared" << '\n';
        cout << "(previously empty)" << '\n';
    } else {
        size_t size = excerptList.size() - 1;
        cout << "excerpt list cleared" << '\n' << "previous contents:" << '\n';
        cout << 0 << '|' << masterLog[static_cast<size_t>(excerptList[0])].entryId << '|' << masterLog[static_cast<size_t>(excerptList[0])].semicolTimestamp << '|' << masterLog[static_cast<size_t>(excerptList[0])].category << '|' << masterLog[static_cast<size_t>(excerptList[0])].messages << '\n';
        cout << "..." << '\n';
        cout << size << '|' << masterLog[static_cast<size_t>(excerptList[size])].entryId << '|' << masterLog[static_cast<size_t>(excerptList[size])].semicolTimestamp << '|' << masterLog[static_cast<size_t>(excerptList[size])].category << '|' << masterLog[static_cast<size_t>(excerptList[size])].messages << '\n';
        excerptList.clear();
    }
}

void printRecent(vector<Log> &masterLog, vector<int> &recentSearch) {
    for (int index : recentSearch) {
        cout << masterLog[static_cast<size_t>(index)].entryId << '|' << masterLog[static_cast<size_t>(index)].semicolTimestamp << '|' << masterLog[static_cast<size_t>(index)].category << '|' << masterLog[static_cast<size_t>(index)].messages << '\n';
    }
}

void printExcerpt(deque<int> &excerptList, vector<Log> &masterLog) {
    for (size_t i = 0; i < excerptList.size(); ++i) {
        cout << i << '|' << masterLog[static_cast<size_t>(excerptList[i])].entryId << '|' << masterLog[static_cast<size_t>(excerptList[i])].semicolTimestamp << '|' << masterLog[static_cast<size_t>(excerptList[i])].category << '|' << masterLog[static_cast<size_t>(excerptList[i])].messages << '\n';
    }
}

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <logfile>" << endl;
        return 1;
    }

    unordered_map<string, vector<int>> categoryMap;
    unordered_map<string, vector<int>> messageMap;
    vector<Log> masterLog;
    ifstream infile;
    infile.open(argv[1]);
    if (!infile.is_open()) {
        cerr << "Error opening file: " << argv[1] << endl;
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            cerr << "Current working directory: " << cwd << endl;
        } else {
            perror("getcwd() error");
        }
        return 1;
    }

    Log input;
    int count = 0;
    while (getline(infile, input.semicolTimestamp, '|')) {
        string temp;
        for (char ch : input.semicolTimestamp) {
            if (ch != ':') {
                temp += ch;
            }
        }
        input.timestamp = stol(temp);
        getline(infile, input.category, '|');
        getline(infile, input.messages);
        input.entryId = count;
        masterLog.push_back(input);
        ++count;
    }
    infile.close();
    cout << count << " entries read" << '\n';

    if (count == 0) {
        cerr << "No entries read from the file. Check the file format." << endl;
        return 1;
    }

    sort(masterLog.begin(), masterLog.end(), masterCompare());

    for (size_t i = 0; i < masterLog.size(); ++i) {
        string temp = convertToLower(masterLog[i].category);
        categoryMap[temp].push_back(static_cast<int>(i));
        makeKeywordMap(masterLog[i], messageMap, static_cast<int>(i));
    }

    vector<int> recentSearch;
    deque<int> excerptList;
    char command;
    cout << "% ";
    while (cin >> command) {
        switch (command) {
            case 't': {
                searchOccurred = true;
                recentSearch.clear();
                string temp1, temp2;
                cin >> ws;
                getline(cin, temp1, '|');
                getline(cin, temp2);
                string temp3, temp4;
                for (char ch : temp1) {
                    if (ch != ':') {
                        temp3 += ch;
                    }
                }
                for (char ch : temp2) {
                    if (ch != ':') {
                        temp4 += ch;
                    }
                }
                long long time1 = stol(temp3);
                long long time2 = stol(temp4);
                timestampSearch(time1, time2, masterLog, recentSearch);
                break;
            }
            case 'm': {
                searchOccurred = true;
                recentSearch.clear();
                string input;
                cin >> input;
                string temp;
                for (char ch : input) {
                    if (ch != ':') {
                        temp += ch;
                    }
                }
                long long timestamp = stol(temp);
                matchingSearch(timestamp, masterLog, recentSearch);
                break;
            }
            case 'c': {
                searchOccurred = true;
                recentSearch.clear();
                string input;
                cin >> ws;
                getline(cin, input);
                transform(input.begin(), input.end(), input.begin(), ::tolower);
                categorySearch(input, categoryMap, recentSearch);
                break;
            }
            case 'k': {
                searchOccurred = true;
                recentSearch.clear();
                string input;
                vector<string> keywords;
                getline(cin, input);
                string temp;
                for (size_t i = 0; i < input.size() + 1; ++i) {
                    if (i == input.size() || input[i] == ' ' || !isalnum(static_cast<unsigned char>(input[i]))) {
                        if (!temp.empty()) {
                            transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
                            keywords.push_back(temp);
                            temp.clear();
                        }
                    } else {
                        temp += input[i];
                    }
                }
                keywordSearch(messageMap, keywords, recentSearch);
                break;
            }
            case 'a': {
                int input;
                cin >> input;
                if (input > static_cast<int>(masterLog.size()) - 1) {
                    cerr << "out of bounds input" << '\n';
                    break;
                }
                appendLog(input, masterLog, excerptList);
                break;
            }
            case 'r': {
                if (!searchOccurred) {
                    cerr << "search has not occurred, invalid print" << '\n';
                    break;
                }
                appendSearch(recentSearch, excerptList);
                break;
            }
            case 'd': {
                size_t input;
                cin >> input;
                deleteLog(input, excerptList);
                break;
            }
            case 'b': {
                size_t input;
                cin >> input;
                moveToBegin(input, excerptList);
                break;
            }
            case 'e': {
                size_t input;
                cin >> input;
                moveToEnd(input, excerptList);
                break;
            }
            case 's': {
                sortExcerpt(excerptList, masterLog);
                break;
            }
            case 'l': {
                clearExcerpt(excerptList, masterLog);
                break;
            }
            case 'g': {
                if (!searchOccurred) {
                    cerr << "search has not occurred, invalid print" << '\n';
                    break;
                }
                printRecent(masterLog, recentSearch);
                break;
            }
            case 'p': {
                printExcerpt(excerptList, masterLog);
                break;
            }
            case 'q': {
                return 0;
            }
            case '#': {
                string input;
                getline(cin, input);
                break;
            }
            default: {
                cerr << "Invalid command" << '\n';
            }
        }
        cout << "% ";
    }
}