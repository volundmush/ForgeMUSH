//
// Created by volund on 10/3/20.
//


#include <stdlib.h>
#include <algorithm>
#include "genarena.h"


namespace forgemush::utils::genarena {
    template<typename T>
    GenArena<T>::GenArena(usize with_capacity) {
        _contents = malloc(sizeof(GenEntry<T>) * with_capacity);
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
    usize GenArena<T>::reserve(usize additional) {
        auto avail = this->available();
        if(avail < additional) {
            auto needed = additional - avail;
            this->_grow(needed);
            return needed;
        }
        return 0;
    }

    template<typename T>
    void GenArena<T>::_grow(usize more) {
        auto old_cap = _capacity;
        _capacity += more;
        if(_capacity > 0 && _capacity > old_cap) {
            _contents = realloc(_contents, (sizeof(GenArena<T>) * _capacity) + more);
            for(usize i = old_cap; i < _capacity; i++) {
                _contents[i] = {0};
                _contents[i].idx = i;
            }
        }
    }

    template<typename T>
    GenEntry<T>* GenArena<T>::_next() {
        GenEntry<T>* out = 0;
        for(usize i = 0; out == 0; i++) {
            if(!_contents[i].active) {
                out = *_contents[i];
            }
        }
        return out;
    }

    template<typename T>
    GenIndex GenArena<T>::insert(T value) {
        if(available() > 0) {
            auto next = _next();
            *next.gen++;
            *next.active = true;
            *next.contents = value;

            GenIndex idx = {.idx = *next.idx, .gen = *next.gen};
            _length++;
            return idx;
        } else {
            _grow(std::max<usize>(1, _capacity / 10));
            return insert(value);
        }
    }

    template<typename T>
    GenEntry<T>* GenArena<T>::_find(GenIndex &idx) {
        GenEntry<T>* out = 0;

        if(idx.idx > _capacity - 1) {
            // this SHOULD exist...
            if(idx.gen == _contents[idx.idx].gen) {
                out = &_contents[idx.idx];
            }
        }

        return out;
    }

    template<typename T>
    bool GenArena<T>::remove(GenIndex &idx) {
        if(auto ptr = _find(idx)) {
            *ptr.active = false;
            *ptr.contents = {0};
            _length--;
            return true;
        } else {
            return false;
        }
    }

    template<typename T>
    T* GenArena<T>::get(GenIndex &idx) {
        T* out = 0;
        if(auto ptr = _find(idx)) {
            out = &*ptr.contents;
        }
        return out;
    }

    template<typename T>
    bool GenArena<T>::has(GenIndex &idx) {
        if(auto ptr = _find(idx)) {
            return true;
        }
        return false;
    }
}
