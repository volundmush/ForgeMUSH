//
// Created by volund on 10/3/20.
//

#ifndef FORGEMUSH_GENARENA_H
#define FORGEMUSH_GENARENA_H

#include <cstdlib>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <list>

namespace forgemush::utils::genarena {

    struct GenIndex {
        size_t bank;
        size_t idx;
        uint64_t gen;
    };

    template<typename T>
    struct GenEntry {
        size_t bank;
        size_t idx;
        uint64_t gen;
        bool active;
        T contents;
    };

    template<typename T>
    class GenBank {
    public:
        GenBank(size_t bank, size_t capacity);
        std::vector<GenEntry<T>> storage;
        std::list<size_t> garbage;
        GenIndex insert(T value);
        T* get(GenIndex idx);
        bool remove(GenIndex idx);
        bool has(GenIndex idx);
        bool available();
    private:
        size_t _bank;
    };

    template<typename T>
    class GenArena {
    public:
        GenArena(size_t start_banks, size_t slots_per_bank);
        ~GenArena();
        GenIndex insert(T value);
        bool remove(GenIndex idx);
        T* get(GenIndex idx);
        bool has(GenIndex idx);
    private:
        size_t _banksize;
        std::vector<GenBank<GenEntry<T>>*> banks;
    };
}




#endif //FORGEMUSH_GENARENA_H
