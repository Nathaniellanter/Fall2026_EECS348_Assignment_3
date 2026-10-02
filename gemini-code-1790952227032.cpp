/*
 * Name of Program: CEO Email Priority Queue System (EECS 348 Assignment 3)
 * Description: Reads commands from standard input or a test file to process and prioritize 
 *              a CEO's emails using a custom list-based MaxHeap priority queue.
 * Inputs: Commands from standard input / file (EMAIL, NEXT, READ, COUNT).
 * Output: Text displayed on standard terminal output.
 * Collaborators: None.
 * Other Sources: ChatGPT-4o and Claude 3.5 Sonnet (used as GenAI comparison baselines).
 * Author's Full Name: Nathaniel Lanter
 * Creation Date: 10/01/2026
 * Revision Date: 10/02/2026
 * Revisions: Refactored GenAI starter code into an object-oriented, list-based MaxHeap 
 *            implementation with full edge-case protection and 100% line commenting.
 */

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

// Class representing an individual Email object
class Email {
public:
    string senderCategory; // Holds sender priority level (Boss, Subordinate, etc.)
    string subjectLine;    // Holds subject line text
    string dateStr;        // Holds date string in MM-DD-YYYY format
    int dateValue;         // Numeric representation of date for chronological comparisons
    int insertionOrder;    // Counter to ensure tie-breaking stability

    // Default constructor
    Email() {
        senderCategory = ""; // Initialize empty sender category
        subjectLine = "";    // Initialize empty subject
        dateStr = "";        // Initialize empty date string
        dateValue = 0;       // Initialize zero date value
        insertionOrder = 0;  // Initialize zero insertion index
    }

    // Parameterized constructor
    Email(string cat, string subj, string dt, int order) {
        senderCategory = cat;        // Set sender category
        subjectLine = subj;          // Set subject line
        dateStr = dt;                // Set raw date string
        insertionOrder = order;      // Set unique insertion sequence number
        dateValue = parseDate(dt);   // Convert string date to integer for sorting
    }

    // Converts MM-DD-YYYY date string to YYYYMMDD integer for direct comparison
    int parseDate(const string& dt) {
        if (dt.length() < 10) return 0; // Return 0 if date format is invalid
        int month = stoi(dt.substr(0, 2)); // Extract 2-digit month
        int day = stoi(dt.substr(3, 2));   // Extract 2-digit day
        int year = stoi(dt.substr(6, 4));  // Extract 4-digit year
        return year * 10000 + month * 100 + day; // Combine into YYYYMMDD integer
    }

    // Maps string categories to integer priority ranks (higher integer = higher priority)
    int getCategoryPriority() const {
        if (senderCategory == "Boss") return 5;             // Boss has highest priority (5)
        if (senderCategory == "Subordinate") return 4;      // Subordinate is second priority (4)
        if (senderCategory == "Peer") return 3;             // Peer is third priority (3)
        if (senderCategory == "ImportantPerson" || 
            senderCategory == "Important Person") return 2; // ImportantPerson is fourth priority (2)
        if (senderCategory == "Other Person" || 
            senderCategory == "OtherPerson") return 1;      // Other Person is lowest priority (1)
        return 0;                                           // Default fallback priority
    }

    // Overloaded greater-than operator to define CEO email reading preference
    bool operator>(const Email& other) const {
        int p1 = getCategoryPriority(); // Priority rank of current email
        int p2 = other.getCategoryPriority(); // Priority rank of comparison email

        if (p1 != p2) {
            return p1 > p2; // Higher category priority comes first
        }
        if (dateValue != other.dateValue) {
            return dateValue > other.dateValue; // Newest email comes first
        }
        return insertionOrder > other.insertionOrder; // Fallback to preserve order
    }

    // Overloaded less-than operator
    bool operator<(const Email& other) const {
        return other > *this; // Reverse of greater-than operator
    }
};

// Node structure for dynamically linked list implementation
struct Node {
    Email data;  // Storage for Email object
    Node* next;  // Pointer to the next node in list

    // Node constructor
    Node(Email e) : data(e), next(nullptr) {} // Construct node with email and null next pointer
};

// Class implementing a MaxHeap priority queue using a list-based data structure
class MaxHeapList {
private:
    Node* head;  // Pointer to the head of linked list
    int count;   // Count of active elements stored in heap

    // Helper method to retrieve an email object at a specific 0-based index
    Email getAt(int index) const {
        Node* curr = head; // Start traversal at head node
        for (int i = 0; i < index && curr != nullptr; ++i) { // Loop until index is reached
            curr = curr->next; // Move to next node
        }
        return curr->data; // Return email at target index
    }

    // Helper method to set an email object at a specific 0-based index
    void setAt(int index, const Email& val) {
        Node* curr = head; // Start traversal at head node
        for (int i = 0; i < index && curr != nullptr; ++i) { // Loop until index is reached
            curr = curr->next; // Move to next node
        }
        if (curr != nullptr) { // Check if node exists
            curr->data = val; // Overwrite data with new email value
        }
    }

    // Percolate-up algorithm to maintain heap property after insertion
    void siftUp(int index) {
        while (index > 0) { // Continue until root is reached
            int parent = (index - 1) / 2; // Calculate parent index
            if (getAt(index) > getAt(parent)) { // If child priority > parent priority
                Email temp = getAt(index); // Temp storage for child
                setAt(index, getAt(parent)); // Move parent down
                setAt(parent, temp); // Move child up
                index = parent; // Set current index to parent index
            } else {
                break; // Heap property satisfied
            }
        }
    }

    // Percolate-down algorithm to maintain heap property after deletion
    void siftDown(int index) {
        while (2 * index + 1 < count) { // While left child exists
            int left = 2 * index + 1; // Left child index
            int right = 2 * index + 2; // Right child index
            int largest = index; // Assume current index is largest

            if (left < count && getAt(left) > getAt(largest)) { // Compare left child
                largest = left; // Update largest index
            }
            if (right < count && getAt(right) > getAt(largest)) { // Compare right child
                largest = right; // Update largest index
            }
            if (largest != index) { // If child is larger than parent
                Email temp = getAt(index); // Temp storage
                setAt(index, getAt(largest)); // Swap parent and child
                setAt(largest, temp); // Complete swap
                index = largest; // Continue down heap
            } else {
                break; // Heap property satisfied
            }
        }
    }

public:
    // MaxHeap constructor
    MaxHeapList() {
        head = nullptr; // Initialize head pointer to null
        count = 0;      // Initialize count to zero
    }

    // Destructor to free allocated memory
    ~MaxHeapList() {
        while (head != nullptr) { // Loop while nodes remain in list
            Node* temp = head; // Store current head
            head = head->next; // Move head to next node
            delete temp;       // Free memory of old head node
        }
    }

    // Inserts a new email into the list-based MaxHeap
    void insert(const Email& email) {
        Node* newNode = new Node(email); // Allocate new list node
        if (head == nullptr) { // If list is empty
            head = newNode; // Set head to new node
        } else {
            Node* curr = head; // Start at head
            while (curr->next != nullptr) { // Loop to find last node
                curr = curr->next; // Move to next node
            }
            curr->next = newNode; // Append new node at end
        }
        count++; // Increment element count
        siftUp(count - 1); // Re-heapify from new element position
    }

    // Returns the highest priority email without removing it
    Email peek() const {
        if (count == 0) return Email(); // Return empty Email if heap is empty
        return head->data; // Return root email stored at list head
    }

    // Removes and returns the highest priority email
    Email extractMax() {
        if (count == 0) return Email(); // Return empty email if heap is empty

        Email maxVal = head->data; // Extract root email data
        Email lastVal = getAt(count - 1); // Get last email in heap

        setAt(0, lastVal); // Move last element to root
        count--; // Decrement count

        if (count == 0) { // If heap becomes empty
            delete head; // Delete root node
            head = nullptr; // Reset head pointer
        } else {
            Node* curr = head; // Start at head
            for (int i = 0; i < count - 1; ++i) { // Traverse to second-to-last node
                curr = curr->next; // Move to next node
            }
            delete curr->next; // Delete old last node
            curr->next = nullptr; // Set new tail pointer to null
            siftDown(0); // Re-heapify from root down
        }

        return maxVal; // Return extracted highest priority email
    }

    // Returns total count of unread emails
    int size() const {
        return count; // Return element count
    }

    // Checks if heap contains no emails
    bool isEmpty() const {
        return count == 0; // Return true if count is 0
    }
};

// Main execution function
int main() {
    MaxHeapList heap; // Instantiate priority queue object
    string line; // String variable to hold line input
    int globalOrder = 0; // Sequence counter to track order of incoming emails

    while (getline(cin, line)) { // Read input line by line
        if (line.empty()) continue; // Skip blank lines

        stringstream ss(line); // Stringstream for command parsing
        string command; // Variable to hold command keyword
        ss >> command; // Read first word as command

        if (command == "EMAIL") { // Handle EMAIL command
            string restOfLine; // String for rest of input
            getline(ss, restOfLine); // Get remaining comma-separated fields

            size_t pos1 = restOfLine.find(','); // Find first comma delimiter
            size_t pos2 = restOfLine.find(',', pos1 + 1); // Find second comma delimiter

            if (pos1 != string::npos && pos2 != string::npos) { // Validate comma presence
                string cat = restOfLine.substr(0, pos1); // Extract category substring
                string subj = restOfLine.substr(pos1 + 1, pos2 - pos1 - 1); // Extract subject substring
                string dt = restOfLine.substr(pos2 + 1); // Extract date substring

                // Trim leading whitespace from category
                while (!cat.empty() && (cat[0] == ' ' || cat[0] == '\t')) cat.erase(0, 1);
                // Trim leading whitespace from subject
                while (!subj.empty() && (subj[0] == ' ' || subj[0] == '\t')) subj.erase(0, 1);
                // Trim leading whitespace from date
                while (!dt.empty() && (dt[0] == ' ' || dt[0] == '\t')) dt.erase(0, 1);

                Email newEmail(cat, subj, dt, ++globalOrder); // Create new Email object
                heap.insert(newEmail); // Insert email into MaxHeap
            }
        } 
        else if (command == "COUNT") { // Handle COUNT command
            cout << "There are " << heap.size() << " emails to read." << endl; // Display count
        } 
        else if (command == "NEXT") { // Handle NEXT command
            if (!heap.isEmpty()) { // Verify emails exist
                Email top = heap.peek(); // Peek top priority email without removing
                cout << "Next email:" << endl; // Output formatted prefix
                cout << "Sender: " << top.senderCategory << endl; // Output sender
                cout << "Subject: " << top.subjectLine << endl;   // Output subject
                cout << "Date: " << top.dateStr << endl;          // Output date
            }
        } 
        else if (command == "READ") { // Handle READ command
            if (!heap.isEmpty()) { // Verify emails exist
                heap.extractMax(); // Remove top priority email from heap
            }
        }
    }

    return 0; // Return success status code
}