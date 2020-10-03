#include <iostream>

#include "typedefs.h"
#include "genarena.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto arena = forgemush::utils::genarena::GenArena<usize>(50);
    arena.insert(600);
    return 0;
}
