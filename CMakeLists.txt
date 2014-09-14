CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
CMAKE_POLICY(SET CMP0003 NEW)

PROJECT(apaclog)

SET(CMAKE_C_FLAGS "-O2 -g -Wall -Wno-unused-function ${CMAKE_C_FLAGS}")

INCLUDE_DIRECTORIES(include)

ADD_LIBRARY(apaclog STATIC lib/parser.c lib/renderer.c)

ADD_EXECUTABLE(examples/dump   examples/dump.c)
ADD_EXECUTABLE(examples/render examples/render.c)

TARGET_LINK_LIBRARIES(examples/dump   apaclog)
TARGET_LINK_LIBRARIES(examples/render apaclog)
