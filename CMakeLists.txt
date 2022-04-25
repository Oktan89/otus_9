cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(bulk VERSION ${PROJECT_VESRION})

option(WITH_GOOGLE_TEST "Whether to build Google tets" OFF)

configure_file(version.h.in version.h)

find_package(Threads)

add_executable(bulk_cli main.cpp)
add_library(async_ver lib.cpp)
add_library(async SHARED 
    async.cpp
    model.cpp
    state.cpp
)

set_target_properties(bulk_cli async_ver async PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(async_ver
    PRIVATE "${CMAKE_BINARY_DIR}"
)

target_link_libraries(async PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries(bulk_cli PRIVATE
    async_ver
    async
)

if(WITH_GOOGLE_TEST)
    add_subdirectory(./googletest EXCLUDE_FROM_ALL)
    add_executable(google_test_version test_version_g.cpp)

    set_target_properties(google_test_version PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )

    target_link_libraries(google_test_version 
        gtest_main 
        gmock
        async_ver
    )
endif()

if (MSVC)
    target_compile_options(bulk PRIVATE
        /W4
    )
    target_compile_options(async_ver PRIVATE
        /W4
    )
    target_compile_options(async PRIVATE
    /W4
    )
    if(WITH_GOOGLE_TEST)
        target_compile_options(google_test_version PRIVATE
            /W4
        )
    endif()
else ()
    target_compile_options(bulk_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(async_ver PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(async PRIVATE
    -Wall -Wextra -pedantic -Werror
    )
    if(WITH_GOOGLE_TEST)
        target_compile_options(google_test_version PRIVATE
        -Wall -Wextra -pedantic -Werror
        )
    endif()
endif()

install(TARGETS bulk_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT r.zhigalenko@mail.ru)
include(CPack)

if(WITH_GOOGLE_TEST)
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(google_test_version)
endif()