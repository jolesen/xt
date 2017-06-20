INCLUDE_DIRECTORIES(${LIB} ${SRC})
LINK_DIRECTORIES(${LIB})
LINK_LIBRARIES(common mysqlclient mongo bson)

FILE(GLOB CPP "${SRC}/data/*.cpp")
ADD_EXECUTABLE(data ${CPP})
