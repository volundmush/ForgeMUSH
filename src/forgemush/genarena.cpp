//
// Created by volund on 11/27/20.
//

#include "../../include/forgemush/genarena.h"

namespace forgemush::utils::genarena {

    template<typename T>
    GenBank<T>::GenBank(size_t bank, size_t capacity) : storage(capacity) {
        _bank = bank;
    }

    template<typename T>
    bool GenBank<T>::available() {
        return !garbage.empty() || storage.capacity() > 0;
    }

    template<typename T>
    T* GenBank<T>::get(GenIndex idx) {

    }

    template<typename T>
    GenArena<T>::GenArena(size_t start_banks, size_t slots_per_bank) {
        _banksize = slots_per_bank;
        for(size_t i = 0;i < start_banks;i++) {
            banks.push_back(new GenBank<T>(i, _banksize));
        }
    }

    template<typename T>
    GenArena<T>::~GenArena() {
        for(const auto b: banks) {
            delete b;
        }
    }

    template<typename T>
    GenIndex GenArena<T>::insert(T value) {
        size_t i = 0;
        for(;i < banks.size();i++) {
            if(banks[i]->available()) {
                return banks[i]->insert(value);
            }
        }
        // Nothing was available? Then create a new bank and insert.
        banks.push_back(new GenBank<T>(i, _banksize));
        return banks[i]->insert(value);
    }

    template<typename T>
    bool GenArena<T>::remove(GenIndex idx) {
        if(idx.bank > banks.size()) {
            return false;
        }
        return banks[idx.bank].remove(idx);
    }

    template<typename T>
    T *GenArena<T>::get(GenIndex idx) {
        if(idx.bank > banks.size()) {
            return false;
        }
        return banks[idx.bank].get(idx);
    }

    template<typename T>
    bool GenArena<T>::has(GenIndex idx) {
        if(idx.bank > banks.size()) {
            return false;
        }
        return banks[idx.bank].has(idx);
    }
}