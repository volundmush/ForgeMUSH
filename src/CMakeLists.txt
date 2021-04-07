#add_library(forgemushcore forgemush/markup.cpp)
#target_include_directories(forgemushcore PUBLIC ../include ${CONAN_INCLUDE_DIRS})
#target_link_libraries(forgemushcore ${CONAN_LIBS})

add_library(forgeportalcore forgeportal/net.cpp ../include/forgeportal/net.h)
target_include_directories(forgeportalcore PUBLIC ../include ${CONAN_INCLUDE_DIRS})
target_link_libraries(forgeportalcore ${CONAN_LIBS})

#FILE(GLOB mudcoreinc ../include/mudcore/*.h)
#FILE(GLOB mudcorecpp mudcore/*.cpp)

#add_library(mudcore ${mudcoreinc} ${mudcorecpp})
#target_compile_options(mudcore PUBLIC -fPIC)
#target_include_directories(mudcore PUBLIC ../include ${CONAN_INCLUDE_DIRS} ${QT_INCLUDES})
#target_link_libraries(mudcore ${CONAN_LIBS})
