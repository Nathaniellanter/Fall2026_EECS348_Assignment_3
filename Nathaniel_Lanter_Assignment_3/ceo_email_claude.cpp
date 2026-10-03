#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Represents a single email in the CEO's inbox.
class Email {
public:
    std::string sender;   // sender category
    std::string subject;  // subject line
    std::string date;     // MM-DD-YYYY
    int rank;             // higher = read sooner
    int dateKey;          // YYYYMMDD, for numeric comparison

    Email() : rank(0), dateKey(0) {}

    Email(const std::string& s, const std::string& subj, const std::string& d)
        : sender(s), subject(subj), date(d) {
        rank = senderRank(s);
        dateKey = makeDateKey(d);
    }

    // Maps sender category to a priority rank.
    static int senderRank(const std::string& s) {
        if (s == "Boss") return 5;
        if (s == "Subordinate") return 4;
        if (s == "Peer") return 3;
        if (s == "ImportantPerson") return 2;
        return 1; // OtherPerson
    }

    // Converts MM-DD-YYYY to the integer YYYYMMDD.
    static int makeDateKey(const std::string& d) {
        int mm = std::stoi(d.substr(0, 2));
        int dd = std::stoi(d.substr(3, 2));
        int yyyy = std::stoi(d.substr(6, 4));
        return yyyy * 10000 + mm * 100 + dd;
    }

    // True if this email has higher priority than other.
    bool higherThan(const Email& other) const {
        if (rank != other.rank) return rank > other.rank;
        return dateKey > other.dateKey; // newer first
    }
};

// Max-heap priority queue implemented on a dynamic array (vector).
class MaxHeap {
private:
    std::vector<Email> data;

    int parent(int i) const { return (i - 1) / 2; }
    int left(int i) const { return 2 * i + 1; }
    int right(int i) const { return 2 * i + 2; }

    void siftUp(int i) {
        while (i > 0 && data[i].higherThan(data[parent(i)])) {
            std::swap(data[i], data[parent(i)]);
            i = parent(i);
        }
    }

    void siftDown(int i) {
        int n = static_cast<int>(data.size());
        while (true) {
            int largest = i;
            int l = left(i), r = right(i);
            if (l < n && data[l].higherThan(data[largest])) largest = l;
            if (r < n && data[r].higherThan(data[largest])) largest = r;
            if (largest == i) break;
            std::swap(data[i], data[largest]);
            i = largest;
        }
    }

public:
    bool empty() const { return data.empty(); }
    int size() const { return static_cast<int>(data.size()); }

    void insert(const Email& e) {
        data.push_back(e);
        siftUp(static_cast<int>(data.size()) - 1);
    }

    const Email& peek() const { return data[0]; }

    void removeMax() {
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) siftDown(0);
    }
};

// Handles reading commands and managing the CEO's inbox.
class CEOInbox {
private:
    MaxHeap heap;

    static std::string trim(const std::string& s) {
        size_t a = s.find_first_not_of(" \t\r\n");
        if (a == std::string::npos) return "";
        size_t b = s.find_last_not_of(" \t\r\n");
        return s.substr(a, b - a + 1);
    }

public:
    void processLine(const std::string& rawLine) {
        std::string line = trim(rawLine);
        if (line.empty()) return;

        if (line.compare(0, 6, "EMAIL ") == 0) {
            std::string rest = line.substr(6);
            std::stringstream ss(rest);
            std::string sender, subject, date;
            std::getline(ss, sender, ',');
            std::getline(ss, subject, ',');
            std::getline(ss, date, ',');
            heap.insert(Email(trim(sender), trim(subject), trim(date)));
        } else if (line == "NEXT") {
            if (heap.empty()) {
                std::cout << "There are no emails to read." << std::endl;
            } else {
                const Email& e = heap.peek();
                std::cout << "Next email:" << std::endl;
                std::cout << "Sender: " << e.sender << std::endl;
                std::cout << "Subject: " << e.subject << std::endl;
                std::cout << "Date: " << e.date << std::endl;
            }
        } else if (line == "READ") {
            if (!heap.empty()) heap.removeMax();
        } else if (line == "COUNT") {
            std::cout << "There are " << heap.size() << " emails to read." << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <test_file>" << std::endl;
        return 1;
    }
    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }
    CEOInbox inbox;
    std::string line;
    while (std::getline(in, line)) inbox.processLine(line);
    return 0;
}
