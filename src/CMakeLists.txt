add_library(libforgemush forgemush/ansi.cpp forgemush/genarena.cpp forgemush/interner.cpp)
target_include_directories(libforgemush PUBLIC ../include)
target_link_libraries(libforgemush ${CONAN_LIBS})

#add_library(libforgeportal)
#target_include_directories(libforgeportal ../include)
#target_link_libraries(libforgeportal ${CONAN_LIBS})