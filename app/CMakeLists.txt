get_all_sources("${CMAKE_CURRENT_SOURCE_DIR}/src" APP_SOURCES)
add_executable(io_test ${APP_SOURCES})

target_link_libraries(io_test PRIVATE my_library)

# Тесты программы
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()