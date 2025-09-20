# Time-Travelling File System

## Overview

This is an implementation of a simplified version control system inspired by Git, built for the COL106 Data Structures and Algorithms course. The system manages versioned files with support for branching and historical inspection using custom implementations of Tree, HashMap, and Heap data structures.

## Features

- **Custom Data Structures**: All required data structures (Tree, HashMap, Heap) implemented from scratch
- **Version Control**: Full version history with snapshot support
- **Branching**: Tree-based version history allows for branching
- **Analytics**: System-wide file metrics using heaps
- **Interactive**: Command-line interface for real-time operations

## Data Structures Implemented

### 1. Tree (TreeNode)
- Represents version history of each file
- Each node contains version information, content, timestamps, and parent-child relationships
- Supports branching through multiple children

### 2. HashMap (HashMap<K,V>)
- Provides O(1) average-time lookups for version IDs
- Uses separate chaining for collision resolution
- Custom hash function with prime-sized table for better distribution

### 3. Heap (Heap<T>)
- Max-heap implementation for system analytics
- Used for tracking most recently modified files and largest version trees
- Configurable comparison function

## File Structure

```
assignment_1/
├── filesystem.cpp      # Main implementation file
├── compile.sh          # Compilation script
├── README.md          # This file
└── COL_106_Long_Assignment_1.pdf  # Assignment specification
```

## Compilation

### Method 1: Using compile script
```bash
./compile.sh
```

### Method 2: Manual compilation
```bash
g++ -std=c++11 filesystem.cpp -o filesystem
```

## Usage

Run the executable:
```bash
./filesystem
```

The system will start an interactive session where you can enter commands from stdin.

## Command Reference

### Core File Operations

1. **CREATE <filename>**
   - Creates a new file with root version (ID 0)
   - Initializes with empty content and snapshot message
   - Example: `CREATE myfile.txt`

2. **READ <filename>**
   - Displays content of the file's currently active version
   - Example: `READ myfile.txt`

3. **INSERT <filename> <content>**
   - Appends content to the file
   - Creates new version if active version is snapshotted
   - Example: `INSERT myfile.txt Hello World`

4. **UPDATE <filename> <content>**
   - Replaces the file's content completely
   - Follows same versioning logic as INSERT
   - Example: `UPDATE myfile.txt New content`

5. **SNAPSHOT <filename> <message>**
   - Marks active version as immutable snapshot
   - Stores provided message and timestamp
   - Example: `SNAPSHOT myfile.txt Initial version`

6. **ROLLBACK <filename> [versionID]**
   - Without versionID: Rolls back to parent of current active version
   - With versionID: Sets active version to specified ID
   - Example: `ROLLBACK myfile.txt 2`

7. **HISTORY <filename>**
   - Lists all snapshotted versions chronologically
   - Shows path from active node to root
   - Displays ID, timestamp, and message
   - Example: `HISTORY myfile.txt`

### System-Wide Analytics

8. **RECENT_FILES [num]**
   - Lists files by last modification time (most recent first)
   - Optional parameter for number of results (default: 10)
   - Example: `RECENT_FILES 5`

9. **BIGGEST_TREES [num]**
   - Lists files by total version count (largest first)
   - Optional parameter for number of results (default: 10)
   - Example: `BIGGEST_TREES 5`

### Special Commands

10. **EXIT**
    - Terminates the program
    - Example: `EXIT`

## Key Semantics

### Immutability
- Only snapshotted versions are immutable
- Non-snapshotted versions can be edited in place
- Once snapshotted, a version cannot be modified

### Versioning
- Version IDs are unique per file
- Assigned sequentially starting from 0
- New versions created automatically when editing snapshotted versions

### Version History
- Tree structure allows for branching
- Each version can have multiple children
- HISTORY command shows path from active version to root

## Error Handling

The system handles various error conditions:

1. **File Operations**:
   - Attempting to create existing file: "Error: File already exists"
   - Accessing non-existent file: "Error: File does not exist"
   - No active version: "Error: No active version for file"

2. **Version Operations**:
   - Invalid version ID: "Error: Version X does not exist"
   - Snapshot on already snapshotted version: "Error: Current version is already a snapshot"
   - Rollback with no parent: "Error: Cannot rollback - no parent version exists"

3. **Command Parsing**:
   - Unknown commands display available options
   - Malformed commands are handled gracefully

## Implementation Details

### Memory Management
- Files are dynamically allocated and should be properly cleaned up
- Tree nodes are managed through parent-child relationships
- HashMap uses vectors for bucket storage

### operations complexity
- HashMap: O(1) average case for lookups, insertions, deletions
- Heap: O(log n) for insertions and deletions
- Tree traversals: O(n) for history display


## Testing

To test the functionality, you can run the following sequence:

```
CREATE test.txt
INSERT test.txt Hello
READ test.txt
SNAPSHOT test.txt First snapshot
INSERT test.txt World
READ test.txt
HISTORY test.txt
ROLLBACK test.txt
READ test.txt
RECENT_FILES
BIGGEST_TREES
EXIT
```

## Dependencies

- C++11 compatible compiler (g++, clang++)
- Standard library headers: iostream, vector, string, ctime, sstream, algorithm

## Notes

- All data structures implemented from scratch as required (Tree, HashMap, Heap only)
- No external data structures used - all analytics deduplication done with vectors
- Interactive command-line interface as specified
- Error handling implemented for all edge cases
- Memory cleanup handled properly in destructors
- Analytics commands properly deduplicate files and show each file only once with its latest information

