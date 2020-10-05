//
// Created by volund on 10/3/20.
//

#ifndef FORGEMUSH_GENARENA_H
#define FORGEMUSH_GENARENA_H

#include "typedefs.h"
#include <stdlib.h>
#include <algorithm>

namespace forgemush::utils::genarena {

    struct GenIndex {
        usize idx;
        u64 gen;
    };

    template<typename T>
    struct GenEntry {
        usize idx;
        u64 gen;
        bool active;
        T contents;
    };

    template<typename T>
    class GenArena {
    public:
        GenArena(usize with_capacity);
        ~GenArena();
        usize available();
        int insert(T value, GenIndex *idx);
        bool remove(GenIndex* idx);
        T* get(GenIndex* idx);
        bool has(GenIndex* idx);
        void iter();
        GenEntry<T>* iter_next();
    private:
        int _grow(usize more);
        GenEntry<T>* _find(GenIndex* idx);
        GenEntry<T>* _next();
        usize _length, _capacity, _iter;
        GenEntry<T>* _contents;
    };


    template<typename T>
    GenArena<T>::GenArena(usize with_capacity) {
        _contents = new GenEntry<T>[with_capacity];
        if(with_capacity > 0) {
            for (usize i = 0; i < with_capacity; i++) {
                _contents[i] = {0};
                _contents[i].idx = i;
            }
        }
        _length = 0;
        _capacity = with_capacity;
    }

    template<typename T>
    GenArena<T>::~GenArena<T>() {
        free(_contents);
    }

    template<typename T>
    usize GenArena<T>::available() {
        return _capacity - _length;
    }

    template<typename T>
    int GenArena<T>::_grow(usize more) {
        auto old_cap = _capacity;
        _capacity += more;
        if(_capacity > 0 && _capacity > old_cap) {
            if(auto bigger = realloc((void*)_contents, (sizeof(GenArena<T>) * _capacity))) {
                _contents = (GenEntry<T>*)bigger;
                for(usize i = old_cap; i < _capacity; i++) {
                    _contents[i] = {0};
                    _contents[i].idx = i;
                }
                return 0;
            } else {
                // Oops.. are we out of memory? THIS IS PROBABABLY BAD.
                return -1;
            }
        } else {
            return 0;
        }
    }

    template<typename T>
    GenEntry<T>* GenArena<T>::_next() {
        GenEntry<T>* out = 0;
        for(usize i = 0; out == 0; i++) {
            if(!_contents[i].active) {
                out = &_contents[i];
            }
        }
        return out;
    }

    template<typename T>
    int GenArena<T>::insert(T value, GenIndex* idx) {
        if(available() > 0) {
            auto next = _next();
            next->gen++;
            next->active = true;
            next->contents = value;

            idx->idx = next->idx;
            idx->gen = next->gen;
            _length++;
            return 0;
        } else {
            if(auto res = _grow(std::max<usize>(1, _capacity / 10))) {
                return res;
            } else {
                return insert(value, idx);
            }

        }
    }

    template<typename T>
    GenEntry<T>* GenArena<T>::_find(GenIndex* idx) {
        GenEntry<T>* out = 0;

        if(idx->idx <= _capacity - 1) {
            // this SHOULD exist...
            if(idx->gen == _contents[idx->idx].gen && _contents[idx->idx].active == true) {
                out = &_contents[idx->idx];
            }
        }

        return out;
    }

    template<typename T>
    bool GenArena<T>::remove(GenIndex* idx) {
        if(auto ptr = _find(idx)) {
            ptr->active = false;
            ptr->contents = {0};
            _length--;
            return true;
        } else {
            return false;
        }
    }

    template<typename T>
    T* GenArena<T>::get(GenIndex* idx) {
        T* out = 0;
        if(auto ptr = _find(idx)) {
            out = &ptr->contents;
        }
        return out;
    }

    template<typename T>
    bool GenArena<T>::has(GenIndex* idx) {
        if(auto ptr = _find(idx)) {
            return true;
        }
        return false;
    }

    template<typename T>
    void GenArena<T>::iter() {
        _iter = 0;
    }

    template<typename T>
    GenEntry<T>* GenArena<T>::iter_next() {
        GenEntry<T>* out = 0;
        while(_iter <= _capacity) {
            if(_contents[_iter].active == true) {
                out = &_contents[_iter];
                _iter++;
                return out;
            }
        }
        return out;
    }

}




#endif //FORGEMUSH_GENARENA_H
