//
// Created by volund on 10/3/20.
//

#ifndef FORGEMUSH_GENARENA_H
#define FORGEMUSH_GENARENA_H

#include "typedefs.h"

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
        explicit GenArena(usize with_capacity);
        ~GenArena();
        usize reserve(usize additional);
        usize available();
        GenIndex insert(T value);
        bool remove(GenIndex &idx);
        T* get(GenIndex &idx);
        bool has(GenIndex &idx);
    private:
        void _grow(usize more);
        GenEntry<T>* _find(GenIndex &idx);
        GenEntry<T>* _next();
        usize _length, _capacity;
        GenEntry<T>* _contents[];
    };

}




#endif //FORGEMUSH_GENARENA_H
