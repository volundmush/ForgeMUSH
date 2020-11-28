add_library(forgemushcore forgemush/ansi.cpp forgemush/genarena.cpp forgemush/interner.cpp)
target_include_directories(forgemushcore PUBLIC ../include)
target_link_libraries(forgemushcore ${CONAN_LIBS})

add_library(forgenet forgenet/net.cpp)
target_include_directories(forgenet PUBLIC ../include)
target_link_libraries(forgenet ${CONAN_LIBS})

#add_library(forgeportalcore)
#target_include_directories(forgeportalcore ../include)
#target_link_libraries(forgeportalcore ${CONAN_LIBS})