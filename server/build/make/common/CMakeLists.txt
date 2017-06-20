INCLUDE_DIRECTORIES(${LIB} ${MONGO} ${BSON} ${SRC})
LINK_DIRECTORIES(${LIB})
LINK_LIBRARIES(event)

FILE(GLOB CPP ${SRC}/common/*.cpp ${SRC}/common/*/*.cpp ${SRC}/common/*/*/*.cpp)
ADD_LIBRARY(common STATIC ${CPP})
