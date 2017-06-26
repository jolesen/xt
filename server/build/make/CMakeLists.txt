PROJECT(xt)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SRC ${CMAKE_SOURCE_DIR}/../../src)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../bin)
SET(LIBRARY_OUTPUT_PATH    ${CMAKE_SOURCE_DIR}/../bin)

INCLUDE_DIRECTORIES(${SRC} /usr/local/libevent/include /usr/local/mysql/include /usr/local/mongo/include/libbson-1.0 /usr/local/mongo/include/libmongoc-1.0)
LINK_DIRECTORIES(/usr/local/libevent/lib /usr/local/mysql/lib /usr/local/mongo/lib)
LINK_LIBRARIES(event mysqlclient mongoc-1.0 bson-1.0)

MESSAGE("\n")
MESSAGE("---------------------------------")
MESSAGE("MAKE TYPE: ${MAKE_TYPE}")
MESSAGE("MONITOR  : ${MONITOR}")
MESSAGE("---------------------------------")
MESSAGE("\n")

# 编译类型
IF(MAKE_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS("-DDEBUG -ggdb -pg -Wall")
    SET(CMAKE_EXE_LINKER_FLAGS "-pg") # gprof, must set linker flag
ELSE()
    ADD_DEFINITIONS("-O2")
ENDIF()

# 是否使用监视器
IF(MONITOR STREQUAL "True")
    ADD_DEFINITIONS("-DUSE_MONITOR")
ENDIF()

# 其他参数
ADD_DEFINITIONS("-std=gnu++0x")
#ADD_DEFINITIONS("-E") #代码宏展开开关

ADD_SUBDIRECTORY(common)
LINK_LIBRARIES(common) # 等同于将下面模块默认添加了这个
ADD_SUBDIRECTORY(center)
ADD_SUBDIRECTORY(auth)
ADD_SUBDIRECTORY(gate)
ADD_SUBDIRECTORY(logic)
ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(robot)
ADD_SUBDIRECTORY(test)
