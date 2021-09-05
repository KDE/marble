# SPDX-FileCopyrightText: 2012-2015 Pavel Klinger
# SPDX-License-Identifier: MIT

project(o5mreader)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(o5mreader_SRCS
    o5mreader.cpp)

add_library(o5mreader STATIC ${o5mreader_SRCS})
