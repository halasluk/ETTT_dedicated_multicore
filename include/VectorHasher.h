#ifndef ETTT_DEDICATED_MULTICORE_VECTORHASHER_H
#define ETTT_DEDICATED_MULTICORE_VECTORHASHER_H

#include <cstdint>
#include <vector>
using namespace std;

struct VectorHasher {
    size_t operator()(vector<int> const& vec) const {
        size_t seed = vec.size();
        for(auto & i : vec) {
            seed ^= (uint32_t)i + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
        }
        return seed;
    }
};

#endif