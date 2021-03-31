//
// Created by volund on 11/26/20.
//

#ifndef FORGEMUSH_DB_H
#define FORGEMUSH_DB_H

#include <entt/entt.hpp>
#include <cstdint>
#include <string>
#include <list>
#include <unordered_set>
#include <unordered_map>

typedef GameObjId uint64_t;
class GameObj;
class GameDataManager;

class LockFlag {

};

class GameObjFlag {
public:
    string name;
    std::unordered_set<GameObjId> objects;
    // TODO: Add virtual functions to make this thing awesome.
    GameObjFlag(string name);
};


class GameObjLock {
public:
    std::string name;
    std::string global_default_value;
    bool system;
    Lock(std::string name, std::string global_default_value, bool system);
};

class ACLEntry {
public:
    GameObjId subject;
    std::string mode;
    uint64_t allow, deny;
};


class GameObjType {
public:
    string name;
    std::unordered_set<GameObjId> objects;
    std::unordered_map<std::string, std::string> lock_defaults;
    GameObjType(string name);
    virtual void CreateObject(std::string name);
    virtual void InitComponents(GameObj* obj);
    virtual void InitACL(GameObj* obj);
    // TODO: Add virtual functions to make this thing awesome.
    // GameObjType governs what GameObjComponents are added to a GameObj when it's setup.
    // It also serves as an index of all Objects of this GameObjType.
    // The GameObjType governs what kinds of locks/ACL permissions are available too.
    // Maybe more stuff.
};

class ObjLock {

};

class CoreComponent {
public:
    GameObjId id;
    entt::entity entid;
    GameObjType *obj_type;
    std::string name;
    std::unordered_set<GameObjFlag*> flags;
    std::unordered_map<GameObjLock*, ObjLock*> locks;
    std::unordered_map<std::string, LockFlag*> lock_flags;
    int64_t creation_timestamp, modification_timestamp;
    std::unordered_map<std::string, GameObjId> relations;
    std::unordered_map<std::string, std::unordered_set<GameObjId>> reverse;
    std::list<ACLEntry> acl;
    std::unordered_set<GameObjId> reverse_acl;
    GameObj(entt:entity entid, GameObjId id; GameObjType* type, std::string name);
};

class CmdQueueComponent {
public:
    GameObjId id;
    entt::entity entid;
};

class IsPlayViewComponent {
public:
    GameObjId id;
    entt::entity entid;
    // TODO: The IsPlayViewComponent bridges Clients and Character GameObjects, providing a view layer.

};

class GridComponent {
public:

};

class IsClientComponent {
public:
    GameObjId id;
    entt::entity entid;
    std::string address;
    uint16_t port;
    std::string protocol;
    bool tls;
};

class IsSessionComponent {
public:
    GameObjId id;
    entt::entity entid;

};

class IsAccountComponent {
public:
    GameObjId id;
    entt::entity entid;

};

class GameDataManager {
public:
    entt::registry registry;
    std::unordered_map<GameObjId, entt::entity> objent;
    std::unordered_map<std::string, GameObjType*> objtypes;
    std::unordered_map<std::string, GameObjFlag*> objflags;

};

#endif //FORGEMUSH_DB_H
