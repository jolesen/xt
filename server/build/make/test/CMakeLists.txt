LINK_LIBRARIES(mysqlclient mongoc-1.0 bson-1.0)

FILE(GLOB CPP "${SRC}/test/*.cpp")
ADD_EXECUTABLE(test ${CPP})
