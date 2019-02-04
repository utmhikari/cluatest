CMAKE_MINIMUM_REQUIRED(VERSION 2.8)    #cmake 最低版本要求，低于2.6 构建过程会被终止。 

PROJECT(cluatest)                          #定义工程名称

SET(CMAKE_BUILD_TYPE DEBUG)                      #指定编译类型
SET(CMAKE_C_FLAGS_DEBUG "-g -Wall")          #指定编译器 

SET(SOURCE_FILES main.c) 

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include /usr/local/include)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/include SRC_FILES)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib /usr/local/lib)
ADD_EXECUTABLE(main  ${SOURCE_FILES} ${SRC_FILES})
TARGET_LINK_LIBRARIES(main liblua.so)
