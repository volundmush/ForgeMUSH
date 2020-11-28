//
// Created by volund on 11/26/20.
//

#ifndef FORGEMUSH_INTERNER_H
#define FORGEMUSH_INTERNER_H

#include <cstdlib>
#include <string>
#include <vector>

// Shittiest String Interner ever. To be replaced with something better.

typedef std::size_t InternIdx;

class StringInterner {
public:
    InternIdx intern(std::string text);
    std::string* retrieve(InternIdx idx);
private:
    std::vector<std::string> storage;
};


#endif //FORGEMUSH_INTERNER_H
