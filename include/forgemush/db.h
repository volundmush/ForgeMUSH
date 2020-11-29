//
// Created by volund on 11/26/20.
//

#ifndef FORGEMUSH_DB_H
#define FORGEMUSH_DB_H

#include <cstdint>
#include <string>
#include <unordered_set>
#include "interner.h"

class DbObjManager;

enum DbRefType : uint8_t {
    Number = 0,
    Name = 1
};

struct NumDbRef {
    DbRefType ref_type;
    uint64_t ref;
};

struct WordDbRef {
    DbRefType ref_type;
    InternIdx idx;
};

union DbRef {
    DbRefType ref_type;
    NumDbRef num_ref;
    WordDbRef word_ref;
};

enum ObjRelType : uint8_t {
    Owner = 0,
    Zone = 1,
    Destination = 2,
    Location = 3,
    Parent = 4,
    Account = 5,
    Namespace = 6
};

enum ObjType : uint8_t {
    Garbage = 0,
    Room = 1,
    Exit = 2,
    Thing = 3,
    Concept = 4
};

struct ObjRel {
    std::optional<DbRef> ref;
    std::unordered_set<DbRef> reverse;
};

struct DbObj {
    DbRef ref;
    ObjType obj_type;
    InternIdx name, uname;
    uint64_t creation_timestamp, modification_timestamp;
    ObjRel relations[7];
};



#endif //FORGEMUSH_DB_H
