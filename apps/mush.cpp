#include <iostream>
#include "forgemush/markup.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    auto m = markup::MarkupString();
    m.decode_markup("\002chr\003Electric\002c/\003");
    auto m2 = markup::MarkupString();
    m2.decode_markup("\002chb\003Boogaloo\002c/\003");
    auto m3 = m + m2;
    auto m4 = m3 + 't';
    auto m5 = m3 + "t2";
    std::cout << "ooga booga" << std::endl;
    std::cin.get();
    return 0;
}
