#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <queue>
#include <algorithm>
#include <math.h>

struct Node {
    unsigned char value{};
    unsigned long long frequency{};
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    std::string code;
};
struct CompareNodeFrequency {
    bool operator()(std::shared_ptr<Node> const& n1, std::shared_ptr<Node> const& n2){
        return n1->frequency > n2->frequency;
    }
};

struct MainBuffer {
    unsigned long long bytes[256];
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, CompareNodeFrequency> minHeap;
    std::vector<std::shared_ptr<Node>> results;
    std::shared_ptr<Node> huffmanRoot;
};

void parseBytes(MainBuffer& buffer, const std::string& file){
    for(unsigned long long& byte : buffer.bytes) { // init array
        byte = 0;
    }
    std::ifstream source(file,std::ios_base::binary);
    int byte;
    bool isFileEmpty = true;
    while (source){
        byte = source.get();
        if (byte < 0) // EOF
            break;
        isFileEmpty = false;
        buffer.bytes[byte]++;
    }
    if (isFileEmpty){
        std::cerr << "Empty file";
        exit(1);
    }
    for (size_t i = 0; i < 256; ++i) {
        if (buffer.bytes[i] > 0){
            auto node = std::make_shared<Node>();
            node->value = i;
            node->frequency = buffer.bytes[i];
            node->left = nullptr;
            node->right = nullptr;
            buffer.minHeap.push(move(node));
        }
    }
}

void buildHuffmanTree(MainBuffer& buffer){
    buffer.huffmanRoot = nullptr;
    while (!buffer.minHeap.empty()){
        if (buffer.minHeap.size() == 1)
            break;
        std::shared_ptr<Node> node = std::make_shared<Node>();
        node->frequency = 0;

        node->left = buffer.minHeap.top();
        buffer.minHeap.pop();
        node->right = buffer.minHeap.top();
        buffer.minHeap.pop();

        node->frequency = node->left->frequency + node->right->frequency;
        buffer.minHeap.push(node);

    }
    buffer.huffmanRoot = buffer.minHeap.top();
    buffer.minHeap.pop();
}

void parseHuffmanTree(std::vector<std::shared_ptr<Node>> &results,std::shared_ptr<Node>& currentNode, std::string code){
    if (currentNode->left){
        code += "0";
        parseHuffmanTree(results,currentNode->left,code);
        code.pop_back();
    }

    if (currentNode->right){
        code += "1";
        parseHuffmanTree(results,currentNode->right,code);
        code.pop_back();
    }
    if (!currentNode->left && !currentNode->right){
        currentNode->code = code;
        results.push_back(currentNode);
    }
}

bool comp(std::shared_ptr<Node> const& n1, std::shared_ptr<Node> const& n2){
    return n1->value < n2->value;
}


int main(int argc, char* argv[]) {
    if (argc != 2)
        return 1;
    std::unique_ptr<Node> ptr = nullptr;
    MainBuffer buffer{};
    parseBytes(buffer, argv[1]);
    buildHuffmanTree(buffer);
    parseHuffmanTree(buffer.results, buffer.huffmanRoot,"");
    std::sort(buffer.results.begin(),buffer.results.end(),comp);
    double check = 0;
    for (const auto& o : buffer.results){
        std::cout << (int)o->value << ": " << o->code << "\n";
        check += 1.0 / std::pow(2,o->code.length());
    }
    std::cout << check;
    return 0;
}
