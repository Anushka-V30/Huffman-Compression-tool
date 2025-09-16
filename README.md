# Huffman Coding File Compressor

A **C++ implementation** of the **Huffman coding algorithm** for **data compression**.  
It includes two utilities:
- **Compressor** → Encodes a file into a compact binary format.  
- **Decompressor** → Restores the original file from its compressed version.

---

##  Features

- **Lossless Compression** – Output is identical to the original file.  
- **Efficient Algorithm** – Priority queue ensures optimal Huffman Tree generation.  
- **Tree Serialization** – Saves Huffman Tree for accurate decompression.  
- **Self-Contained** – Pure C++ with no external dependencies.  

---

## 🧠 How It Works

Huffman coding assigns **variable-length binary codes** to characters based on frequency:  
- Frequent characters → Shorter codes  
- Rare characters → Longer codes  

This minimizes the overall file size.

---

### 🔹 Compression Process (`compressor.cpp`)

1. **Frequency Analysis** – Count frequency of each character.  
2. **Build Huffman Tree** –  
   - Use a min-heap to merge lowest-frequency nodes.  
   - Repeat until one root node remains.  
3. **Generate Codes** – Traverse the tree to assign binary codes.  
4. **Serialize Tree** – Save tree structure in `.tree` file.  
5. **Encode File** – Replace characters with Huffman codes.  
6. **Write Binary Data** –  
   - Pad bitstring to multiple of 8.  
   - Store number of padding bits in the first byte of `.huff` file.  
   - Save compressed binary output.  

---

### 🔹 Decompression Process (`decompressor.cpp`)

1. **Rebuild Tree** – Load Huffman Tree from `.tree` file.  
2. **Read Compressed Data** –  
   - Get padding count from first byte.  
   - Convert remaining bytes back into bitstream.  
3. **Decode Bitstream** –  
   - Traverse tree bit by bit.  
   - Write decoded characters into output file.  
   - Reset to root after reaching each leaf node.  


