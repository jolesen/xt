LINK_LIBRARIES(mysqlclient mongoc-1.0 bson-1.0)

FILE(GLOB CPP "${SRC}/data/*.cpp")
ADD_EXECUTABLE(data ${CPP})
