FILE(GLOB CPP ${SRC}/common/*.cpp ${SRC}/common/*/*.cpp ${SRC}/common/*/*/*.cpp)
ADD_LIBRARY(common STATIC ${CPP})
