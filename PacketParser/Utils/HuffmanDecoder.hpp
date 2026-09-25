#pragma once

#include <vector>
#include <cstdint>
#include <memory>

namespace PacketParser {
namespace Utils {

class HuffmanDecoder {
public:
    class Node {
    private:
        int symbol;
        int bits;
        std::vector<std::shared_ptr<Node>> children;

    public:
        Node();
        Node(int symbol, int bits);

        int Symbol() const { return symbol; }
        int Bits() const { return bits; }
        const std::vector<std::shared_ptr<Node>>& Children() const { return children; }
        std::vector<std::shared_ptr<Node>>& Children() { return children; }

        bool IsTerminal() const;
    };

private:
    static const int HUFFMAN_EOS = 256;
    std::shared_ptr<Node> root;

    static std::shared_ptr<Node> BuildTree(const std::vector<int>& codes, const std::vector<uint8_t>& lengths);
    static void Insert(std::shared_ptr<Node> root, int symbol, int code, uint8_t length);

public:
    static const std::vector<int> HPACK_HUFFMAN_CODES;
    static const std::vector<uint8_t> HPACK_HUFFMAN_CODE_LENGTHS;

    HuffmanDecoder();
    HuffmanDecoder(const std::vector<int>& codes, const std::vector<uint8_t>& lengths);

    std::vector<uint8_t> Decode(const std::vector<uint8_t>& buf) const;
};

} // namespace Utils
} // namespace PacketParser
