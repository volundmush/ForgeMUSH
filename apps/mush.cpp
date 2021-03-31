#include <iostream>
#include "forgemush/markup.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    auto m = markup::MarkupString();
    auto s = std::string("\002chr\003Boogaloo\002c/\003");
    m.decode_markup(s);
    std::cout << "ooga booga" << std::endl;
    std::cin.get();
    return 0;
}
