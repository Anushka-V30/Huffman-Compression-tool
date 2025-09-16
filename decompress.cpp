#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>

// Represents a node in the Huffman tree (frequency is not needed for decompression)
struct HuffmanNode {
    char character;
    HuffmanNode *left, *right;

    HuffmanNode(char c = '\0') : character(c), left(nullptr), right(nullptr) {}
};

/**
 * @brief Rebuilds the Huffman tree by reading the serialized tree file.
 * @param treeFileStream The input file stream of the serialized tree.
 * @return A pointer to the root of the reconstructed Huffman tree.
 */
HuffmanNode* deserializeHuffmanTree(std::ifstream& treeFileStream) {
    char nodeTypeFlag;
    treeFileStream.get(nodeTypeFlag);

    if (nodeTypeFlag == '1') { // This is a leaf node
        char character;
        treeFileStream.get(character);
        return new HuffmanNode(character);
    }
    
    // This is an internal node
    auto* node = new HuffmanNode();
    node->left = deserializeHuffmanTree(treeFileStream);
    node->right = deserializeHuffmanTree(treeFileStream);
    return node;
}

/**
 * @brief Decompresses a file using a Huffman tree.
 * @param sourcePath The path to the compressed input file.
 * @param destinationPath The path where the decompressed output file will be saved.
 */
void huffmanDecodeFile(const std::string& sourcePath, const std::string& destinationPath) {
    std::ifstream inputFileStream(sourcePath, std::ios::binary);
    if (!inputFileStream) {
        std::cerr << "Error: Cannot open input file: " << sourcePath << std::endl;
        return;
    }

    std::ifstream treeFileStream(sourcePath + ".tree", std::ios::binary);
    if (!treeFileStream) {
        std::cerr << "Error: Cannot open tree file: " << sourcePath + ".tree" << std::endl;
        return;
    }
    
    std::ofstream outputFileStream(destinationPath, std::ios::binary);

    // --- Step 1: Rebuild the Huffman Tree from the .tree file ---
    HuffmanNode* treeRoot = deserializeHuffmanTree(treeFileStream);
    treeFileStream.close();

    // --- Step 2: Read the encoded file and convert it to a bit string ---
    // The first byte tells us how many padding bits were added to the end.
    char paddingBitsChar;
    inputFileStream.get(paddingBitsChar);
    const int paddingBits = static_cast<int>(paddingBitsChar);

    std::string encodedBitString;
    char byte;
    while (inputFileStream.get(byte)) {
        encodedBitString += std::bitset<8>(static_cast<unsigned char>(byte)).to_string();
    }
    inputFileStream.close();

    // Remove the padding bits from the end of the string, if any.
    if (paddingBits > 0) {
        encodedBitString.resize(encodedBitString.size() - paddingBits);
    }
    
    if (treeRoot == nullptr) { // Handle case of empty source file
         std::cout << "Decompression complete (input was empty).\n";
         outputFileStream.close();
         return;
    }

    // --- Step 3: Decode the bit string using the tree ---
    HuffmanNode* currentNode = treeRoot;
    for (const char& bit : encodedBitString) {
        // Move left for '0' and right for '1'
        currentNode = (bit == '0') ? currentNode->left : currentNode->right;

        // If a leaf node is reached, we have found a character
        if (!currentNode->left && !currentNode->right) {
            outputFileStream.put(currentNode->character);
            currentNode = treeRoot; // Reset to the root for the next character
        }
    }
    outputFileStream.close();

    std::cout << "Decompression complete. Output saved to: " << destinationPath << std::endl;
}

int main() {
    // These file names should correspond to the output of the compression program
    const std::string compressedFile = "compressed_output.huff";
    const std::string decompressedFile = "decompressed_original.txt";

    huffmanDecodeFile(compressedFile, decompressedFile);

    return 0;
}