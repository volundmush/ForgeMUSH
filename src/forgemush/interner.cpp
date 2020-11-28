//
// Created by volund on 11/26/20.
//

#include "../../include/forgemush/interner.h"

InternIdx StringInterner::intern(std::string text) {
    InternIdx i = 0;
    for(;i < storage.size();i++) {
        if(storage[i] == text) {
            return i;
        }
    }
    storage.push_back(text);
    return i;
}

std::string* StringInterner::retrieve(InternIdx idx) {
    if(idx > storage.size()) {
        throw "string interner out of range";
    }
    return &this->storage[idx];
}