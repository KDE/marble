# SPDX-FileCopyrightText: 2023 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

find_program(OSMCONVERT_EXECUTABLE NAMES osmconvert)
if (NOT OSMCONVERT_EXECUTABLE)
    include(FetchContent)
    message(STATUS "Building external osmconvert for tile generator tests!")
    FetchContent_Declare(osm-c-tools
        GIT_REPOSITORY https://gitlab.com/osm-c-tools/osmctools.git
    )
    FetchContent_Populate(osm-c-tools)
    add_executable(osmconvert ${osm-c-tools_SOURCE_DIR}/src/osmconvert.c)
    set_target_properties(osmconvert PROPERTIES C_STANDARD 99)
    target_link_libraries(osmconvert PRIVATE ZLIB::ZLIB)
endif()

add_test(NAME tirex-backend-test
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/tile-creator-tester.py
        --tirex-backend $<TARGET_FILE:marble-vectorosm-tirex-backend>
        --osmconvert $<TARGET_FILE:osmconvert>
        --data ${CMAKE_CURRENT_SOURCE_DIR}/data
)
set_tests_properties(tirex-backend-test PROPERTIES
    ENVIRONMENT_MODIFICATION "PATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}"
)
