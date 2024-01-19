#pragma once
#include <array>
#include <unordered_map>

// Custom hash for std::array<int, 2>
struct ArrayHash {
    size_t operator()(const std::array<int, 2>& arr) const {
        std::hash<int> hasher;
        size_t seed = 0;
        seed ^= hasher(arr[0]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hasher(arr[1]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};
