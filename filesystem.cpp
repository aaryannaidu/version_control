#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <algorithm>

using namespace std;

// Forward declarations
struct TreeNode;
struct File;

// HashMap Implementation (using separate chaining for collision resolution)
template<typename K, typename V>
class HashMap {
private:
    static const int TABLE_SIZE = 10007; // Prime number for better distribution
    vector<vector<pair<K, V>>> table;

    int hashFunction(const K& key) const {
        return hash<K>{}(key) % TABLE_SIZE;
    }

public:
    HashMap() {
        table.resize(TABLE_SIZE);
    }

    void put(const K& key, const V& value) {
        int index = hashFunction(key);
        for (auto& pair : table[index]) {
            if (pair.first == key) {
                pair.second = value;
                return;
            }
        }
        table[index].push_back({key, value});
    }

    V* get(const K& key) {
        int index = hashFunction(key);
        for (auto& pair : table[index]) {
            if (pair.first == key) {
                return &pair.second;
            }
        }
        return nullptr;
    }

    bool contains(const K& key) {
        return get(key) != nullptr;
    }

    void remove(const K& key) {
        int index = hashFunction(key);
        auto& bucket = table[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                bucket.erase(it);
                return;
            }
        }
    }
};

// Heap Implementation (Max Heap for analytics)
template<typename T>
class Heap {
private:
    vector<T> heap;
    bool (*compare)(const T&, const T&);

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (compare(heap[index], heap[parent])) {
                swap(heap[index], heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int index) {
        int size = heap.size();
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size && compare(heap[left], heap[largest])) {
                largest = left;
            }
            if (right < size && compare(heap[right], heap[largest])) {
                largest = right;
            }

            if (largest != index) {
                swap(heap[index], heap[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }

public:
    Heap(bool (*comp)(const T&, const T&)) : compare(comp) {}

    void push(const T& item) {
        heap.push_back(item);
        heapifyUp(heap.size() - 1);
    }

    T pop() {
        if (heap.empty()) {
            throw runtime_error("Heap is empty");
        }
        T root = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            heapifyDown(0);
        }
        return root;
    }

    const T& top() const {
        if (heap.empty()) {
            throw runtime_error("Heap is empty");
        }
        return heap[0];
    }

    bool empty() const {
        return heap.empty();
    }

    size_t size() const {
        return heap.size();
    }
};

// Tree Node Structure
struct TreeNode {
    int version_id;
    string content;
    string message;
    time_t created_timestamp;
    time_t snapshot_timestamp;
    TreeNode* parent;
    vector<TreeNode*> children;

    TreeNode(int id, const string& cont = "", TreeNode* par = nullptr)
        : version_id(id), content(cont), message(""), parent(par),
          created_timestamp(time(nullptr)), snapshot_timestamp(0) {}

    bool isSnapshot() const {
        return snapshot_timestamp != 0;
    }
};

// File Structure
struct File {
    TreeNode* root;
    TreeNode* active_version;
    HashMap<int, TreeNode*> version_map;
    int total_versions;
    time_t last_modified;

    File() : root(nullptr), active_version(nullptr), total_versions(0), last_modified(0) {}

    ~File() {
        // Clean up memory (recursive deletion)
        if (root) {
            deleteTree(root);
        }
    }

private:
    void deleteTree(TreeNode* node) {
        if (!node) return;
        for (TreeNode* child : node->children) {
            deleteTree(child);
        }
        delete node;
    }
};

// File System Manager
class FileSystem {
private:
    HashMap<string, File*> files;
    Heap<pair<time_t, string>> recentFilesHeap;
    Heap<pair<int, string>> biggestTreesHeap;

    // Comparison functions for heaps
    static bool compareRecent(const pair<time_t, string>& a, const pair<time_t, string>& b) {
        return a.first > b.first; // Max heap by time (most recent first)
    }

    static bool compareBiggest(const pair<int, string>& a, const pair<int, string>& b) {
        return a.first > b.first; // Max heap by version count
    }

public:
    FileSystem()
        : recentFilesHeap(compareRecent), biggestTreesHeap(compareBiggest) {}

    ~FileSystem() {
        // Clean up all files - implement proper cleanup
    }

    // Core File Operations
    void createFile(const string& filename) {
        if (files.contains(filename)) {
            cout << "Error: File '" << filename << "' already exists." << endl;
            return;
        }

        File* newFile = new File();
        TreeNode* rootNode = new TreeNode(0);
        rootNode->snapshot_timestamp = time(nullptr);
        rootNode->message = "Initial snapshot";

        newFile->root = rootNode;
        newFile->active_version = rootNode;
        newFile->version_map.put(0, rootNode);
        newFile->total_versions = 1;
        newFile->last_modified = time(nullptr);

        files.put(filename, newFile);
        updateAnalytics(filename, newFile);
        cout << "File '" << filename << "' created successfully." << endl;
    }

    void readFile(const string& filename) {
        File** filePtr = files.get(filename);
        if (!filePtr || !*filePtr) {
            cout << "Error: File '" << filename << "' does not exist." << endl;
            return;
        }

        File* file = *filePtr;
        if (!file->active_version) {
            cout << "Error: No active version for file '" << filename << "'." << endl;
            return;
        }

        cout << file->active_version->content << endl;
    }

    void insertContent(const string& filename, const string& content) {
        File** filePtr = files.get(filename);
        if (!filePtr || !*filePtr) {
            cout << "Error: File '" << filename << "' does not exist." << endl;
            return;
        }

        File* file = *filePtr;
        if (file->active_version->isSnapshot()) {
            // Create new version
            TreeNode* newNode = new TreeNode(file->total_versions);
            newNode->content = file->active_version->content + content;
            newNode->parent = file->active_version;
            file->active_version->children.push_back(newNode);
            file->active_version = newNode;
            file->version_map.put(newNode->version_id, newNode);
            file->total_versions++;
        } else {
            // Modify current version in place
            file->active_version->content += content;
        }

        file->last_modified = time(nullptr);
        updateAnalytics(filename, file);
    }

    void updateContent(const string& filename, const string& content) {
        File** filePtr = files.get(filename);
        if (!filePtr || !*filePtr) {
            cout << "Error: File '" << filename << "' does not exist." << endl;
            return;
        }

        File* file = *filePtr;
        if (file->active_version->isSnapshot()) {
            // Create new version
            TreeNode* newNode = new TreeNode(file->total_versions);
            newNode->content = content;
            newNode->parent = file->active_version;
            file->active_version->children.push_back(newNode);
            file->active_version = newNode;
            file->version_map.put(newNode->version_id, newNode);
            file->total_versions++;
        } else {
            // Modify current version in place
            file->active_version->content = content;
        }

        file->last_modified = time(nullptr);
        updateAnalytics(filename, file);
    }

    void createSnapshot(const string& filename, const string& message) {
        File** filePtr = files.get(filename);
        if (!filePtr || !*filePtr) {
            cout << "Error: File '" << filename << "' does not exist." << endl;
            return;
        }

        File* file = *filePtr;
        if (file->active_version->isSnapshot()) {
            cout << "Error: Current version is already a snapshot." << endl;
            return;
        }

        file->active_version->snapshot_timestamp = time(nullptr);
        file->active_version->message = message;
        file->last_modified = time(nullptr);
        updateAnalytics(filename, file);
    }

    void rollback(const string& filename, int versionId = -1) {
        File** filePtr = files.get(filename);
        if (!filePtr || !*filePtr) {
            cout << "Error: File '" << filename << "' does not exist." << endl;
            return;
        }

        File* file = *filePtr;
        if (versionId == -1) {
            // Rollback to parent
            if (!file->active_version->parent) {
                cout << "Error: Cannot rollback - no parent version exists." << endl;
                return;
            }
            file->active_version = file->active_version->parent;
        } else {
            // Rollback to specific version
            TreeNode** nodePtr = file->version_map.get(versionId);
            if (!nodePtr || !*nodePtr) {
                cout << "Error: Version " << versionId << " does not exist." << endl;
                return;
            }
            file->active_version = *nodePtr;
        }

        file->last_modified = time(nullptr);
        updateAnalytics(filename, file);
    }

    void showHistory(const string& filename) {
        File** filePtr = files.get(filename);
        if (!filePtr || !*filePtr) {
            cout << "Error: File '" << filename << "' does not exist." << endl;
            return;
        }

        File* file = *filePtr;
        vector<TreeNode*> path;

        // Build path from active version to root
        TreeNode* current = file->active_version;
        while (current) {
            if (current->isSnapshot()) {
                path.push_back(current);
            }
            current = current->parent;
        }

        // Display in chronological order (from oldest to newest)
        reverse(path.begin(), path.end());

        cout << "History for file '" << filename << "':" << endl;
        for (TreeNode* node : path) {
            char timeStr[26];
            ctime_r(&node->snapshot_timestamp, timeStr);
            timeStr[strlen(timeStr) - 1] = '\0'; 

            cout << "Version " << node->version_id << " - " << timeStr
                 << " - " << node->message << endl;
        }
    }

    void showRecentFiles(int num = 10) {
        vector<pair<time_t, string>> tempHeap;
        // Copy heap to vector for easier manipulation
        while (!recentFilesHeap.empty()) {
            tempHeap.push_back(recentFilesHeap.pop());
        }

        // Remove duplicates by keeping only the most recent timestamp for each file
        vector<pair<time_t, string>> uniqueFiles;

        for (auto& item : tempHeap) {
            bool found = false;
            // Check if this file already exists in uniqueFiles
            for (size_t i = 0; i < uniqueFiles.size(); ++i) {
                if (uniqueFiles[i].second == item.second) {
                    // Update if this timestamp is more recent
                    if (uniqueFiles[i].first < item.first) {
                        uniqueFiles[i].first = item.first;
                    }
                    found = true;
                    break;
                }
            }
            // If file not found, add it
            if (!found) {
                uniqueFiles.push_back(item);
            }
        }

        // Sort by timestamp (descending - most recent first)
        sort(uniqueFiles.begin(), uniqueFiles.end(),
             [](const pair<time_t, string>& a, const pair<time_t, string>& b) {
                 return a.first > b.first;
             });

        // Restore heap
        for (auto& item : tempHeap) {
            recentFilesHeap.push(item);
        }

        cout << "Recent files:" << endl;
        int count = 0;
        for (auto& item : uniqueFiles) {
            if (count >= num) break;
            char timeStr[26];
            ctime_r(&item.first, timeStr);
            timeStr[strlen(timeStr) - 1] = '\0';
            cout << item.second << " - " << timeStr << endl;
            count++;
        }
    }

    void showBiggestTrees(int num = 10) {
        vector<pair<int, string>> tempHeap;
        // Copy heap to vector for easier manipulation
        while (!biggestTreesHeap.empty()) {
            tempHeap.push_back(biggestTreesHeap.pop());
        }

        // Remove duplicates by keeping only the highest version count for each file
        vector<pair<int, string>> uniqueFiles;

        for (auto& item : tempHeap) {
            bool found = false;
            // Check if this file already exists in uniqueFiles
            for (size_t i = 0; i < uniqueFiles.size(); ++i) {
                if (uniqueFiles[i].second == item.second) {
                    // Update if this version count is higher
                    if (uniqueFiles[i].first < item.first) {
                        uniqueFiles[i].first = item.first;
                    }
                    found = true;
                    break;
                }
            }
            // If file not found, add it
            if (!found) {
                uniqueFiles.push_back(item);
            }
        }

        // Sort by version count (descending - highest first)
        sort(uniqueFiles.begin(), uniqueFiles.end(),
             [](const pair<int, string>& a, const pair<int, string>& b) {
                 return a.first > b.first;
             });

        // Restore heap
        for (auto& item : tempHeap) {
            biggestTreesHeap.push(item);
        }

        cout << "Biggest trees:" << endl;
        int count = 0;
        for (auto& item : uniqueFiles) {
            if (count >= num) break;
            cout << item.second << " - " << item.first << " versions" << endl;
            count++;
        }
    }

private:
    void updateAnalytics(const string& filename, File* file) {
        // Note: We push entries to heaps on every update. The display functions
        // handle deduplication using vector-based approach (no unordered_map).

        // Update recent files heap
        recentFilesHeap.push({file->last_modified, filename});

        // Update biggest trees heap
        biggestTreesHeap.push({file->total_versions, filename});
    }
};

// Command Parser and Main Function
int main() {
    FileSystem fs;
    string line;

    cout << "Time-Travelling File System" << endl;


    while (getline(cin, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string command;
        ss >> command;

        if (command == "EXIT") {
            break;
        } else if (command == "CREATE") {
            string filename;
            ss >> filename;
            fs.createFile(filename);
        } else if (command == "READ") {
            string filename;
            ss >> filename;
            fs.readFile(filename);
        } else if (command == "INSERT") {
            string filename, content;
            ss >> filename;
            getline(ss, content);
            // Remove leading space if any
            if (!content.empty() && content[0] == ' ') {
                content = content.substr(1);
            }
            fs.insertContent(filename, content);
        } else if (command == "UPDATE") {
            string filename, content;
            ss >> filename;
            getline(ss, content);
            if (!content.empty() && content[0] == ' ') {
                content = content.substr(1);
            }
            fs.updateContent(filename, content);
        } else if (command == "SNAPSHOT") {
            string filename, message;
            ss >> filename;
            getline(ss, message);
            if (!message.empty() && message[0] == ' ') {
                message = message.substr(1);
            }
            fs.createSnapshot(filename, message);
        } else if (command == "ROLLBACK") {
            string filename;
            int versionId = -1;
            ss >> filename;
            if (ss >> versionId) {
                // Version ID provided
            }
            fs.rollback(filename, versionId);
        } else if (command == "HISTORY") {
            string filename;
            ss >> filename;
            fs.showHistory(filename);
        } else if (command == "RECENT_FILES") {
            int num = 10;
            if (ss >> num) {
                // Number provided
            }
            fs.showRecentFiles(num);
        } else if (command == "BIGGEST_TREES") {
            int num = 10;
            if (ss >> num) {
                // Number provided
            }
            fs.showBiggestTrees(num);
        } else {
            cout << "Unknown command: " << command << endl;
            cout << "Available commands: CREATE, READ, INSERT, UPDATE, SNAPSHOT, ROLLBACK, HISTORY, RECENT_FILES, BIGGEST_TREES, EXIT" << endl;
        }
    }

    return 0;
}
