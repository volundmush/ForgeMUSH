#include <iostream>

#include "typedefs.h"
#include "genarena.h"
#include "dbdata.h"

struct Vector2 {
    int x, y;
};

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto arena = new forgemush::utils::genarena::GenArena<Vector2>(50);
    forgemush::utils::genarena::GenIndex idx = {0};
    Vector2 v2 = {.x = 50, .y = -60};
    auto ins = arena->insert(v2, &idx);
    auto res = arena->has(&idx);
    auto something = arena->get(&idx);
    auto rem = arena->remove(&idx);
    auto h = arena->has(&idx);
    something = arena->get(&idx);
    return 0;
}
