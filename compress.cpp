#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <bitset>
#include <streambuf>

// Represents a node in the Huffman tree
struct HuffmanNode {
    char character;
    int frequency;
    HuffmanNode *left, *right;

    HuffmanNode(char c, int freq) : character(c), frequency(freq), left(nullptr), right(nullptr) {}
};

// Custom comparator for the priority queue to build a min-heap based on frequency
struct NodeComparator {
    bool operator()(const HuffmanNode* a, const HuffmanNode* b) const {
        return a->frequency > b->frequency;
    }
};

/**
 * @brief Recursively traverses the Huffman tree to generate the binary codes for each character.
 * @param root The current node in the Huffman tree.
 * @param huffmanCodeTable A map to store the generated character-to-code mappings.
 * @param codeInProgress The binary code string being built during traversal.
 */
void generateHuffmanCodes(HuffmanNode* root, std::unordered_map<char, std::string>& huffmanCodeTable, const std::string& codeInProgress) {
    if (!root) {
        return;
    }
    // A leaf node contains a character
    if (!root->left && !root->right) {
        huffmanCodeTable[root->character] = codeInProgress;
    }
    generateHuffmanCodes(root->left, huffmanCodeTable, codeInProgress + "0");
    generateHuffmanCodes(root->right, huffmanCodeTable, codeInProgress + "1");
}

/**
 * @brief Serializes the Huffman tree structure to a file for later decompression.
 * @param root The root of the Huffman tree.
 * @param treeFileStream The output file stream for the tree data.
 */
void serializeHuffmanTree(const HuffmanNode* root, std::ofstream& treeFileStream) {
    if (!root) {
        return;
    }
    // Mark leaf nodes with '1' followed by the character
    if (!root->left && !root->right) {
        treeFileStream << '1' << root->character;
    } else { // Mark internal nodes with '0'
        treeFileStream << '0';
        serializeHuffmanTree(root->left, treeFileStream);
        serializeHuffmanTree(root->right, treeFileStream);
    }
}

/**
 * @brief Compresses a file using the Huffman coding algorithm.
 * @param sourcePath The path to the input file to be compressed.
 * @param destinationPath The path where the compressed output file will be saved.
 */
void huffmanEncodeFile(const std::string& sourcePath, const std::string& destinationPath) {
    std::ifstream inputFileStream(sourcePath, std::ios::binary);
    if (!inputFileStream) {
        std::cerr << "Error: Cannot open input file: " << sourcePath << std::endl;
        return;
    }

    std::ofstream outputFileStream(destinationPath, std::ios::binary);
    std::ofstream treeFileStream(destinationPath + ".tree", std::ios::binary);

    // 1. Read the file and calculate character frequencies
    std::unordered_map<char, int> frequencyMap;
    const std::string fileData((std::istreambuf_iterator<char>(inputFileStream)), std::istreambuf_iterator<char>());
    inputFileStream.close();

    // 2. Build the Huffman tree using a priority queue
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeComparator> minHeap;
    for (const auto& pair : frequencyMap) {
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    }

    while (minHeap.size() > 1) {
        HuffmanNode* leftChild = minHeap.top();
        minHeap.pop();
        HuffmanNode* rightChild = minHeap.top();
        minHeap.pop();
        
        HuffmanNode* parentNode = new HuffmanNode('\0', leftChild->frequency + rightChild->frequency);
        parentNode->left = leftChild;
        parentNode->right = rightChild;
        minHeap.push(parentNode);
    }
    HuffmanNode* treeRoot = minHeap.top();

    // 3. Save the tree structure to a separate file for decompression
    serializeHuffmanTree(treeRoot, treeFileStream);
    treeFileStream.close();

    // 4. Generate the Huffman codes for each character
    std::unordered_map<char, std::string> huffmanCodeTable;
    generateHuffmanCodes(treeRoot, huffmanCodeTable, "");

    // 5. Encode the original file content into a bit string
    std::string bitString = "";
    for (const char& c : fileData) {
        bitString += huffmanCodeTable[c];
    }

    // 6. Pad the bit string to make its length a multiple of 8
    // The first byte of the compressed file stores the number of padding bits
    int paddingBits = (8 - bitString.length() % 8) % 8;
    outputFileStream.put(static_cast<char>(paddingBits));
    
    for (int i = 0; i < paddingBits; ++i) {
        bitString += '0';
    }

    // 7. Write the encoded data to the output file byte by byte
    for (size_t i = 0; i < bitString.length(); i += 8) {
        std::bitset<8> byte(bitString.substr(i, 8));
        outputFileStream.put(static_cast<unsigned char>(byte.to_ulong()));
    }

    outputFileStream.close();
    std::cout << "Compression complete. Output saved to: " << destinationPath << std::endl;
}


int main() {
    // === IMPORTANT: Change this line to the path of your input file ===
    const std::string sourceFile = "input.txt"; 
    
    const std::string compressedFile = "compressed_output.huff";
    
    huffmanEncodeFile(sourceFile, compressedFile);
    
    return 0;
}