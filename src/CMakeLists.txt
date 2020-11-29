add_library(forgemushcore forgemush/ansi.cpp forgemush/genarena.cpp forgemush/interner.cpp)
target_include_directories(forgemushcore PUBLIC ../include ${CONAN_INCLUDE_DIRS})
target_link_libraries(forgemushcore ${CONAN_LIBS})

add_library(forgeportalcore forgeportal/net.cpp forgeportal/telnet.cpp)
target_include_directories(forgeportalcore PUBLIC ../include ${CONAN_INCLUDE_DIRS})
target_link_libraries(forgeportalcore ${CONAN_LIBS})