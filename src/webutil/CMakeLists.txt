# Configure target sources here
set(WEBUTIL_SRC_FILES src/http.cpp src/path.cpp src/hash.cpp)
set(WEBUTIL_TESTS_SRC_FILES test/webutil_tests.cpp test/http_test.cpp test/path_test.cpp test/hash_test.cpp)


# webutil library
add_library(webutil ${WEBUTIL_SRC_FILES})
target_include_directories(webutil PUBLIC ./include ${BOTAN_PKG_INCLUDE_DIRS} ${PROJECT_ROOT})
target_link_libraries(webutil OpenSSL::SSL OpenSSL::Crypto Boost::thread ${BOTAN_PKG_LIBRARIES})

# webutil library tests
add_executable(webutil_tests ${WEBUTIL_TESTS_SRC_FILES})
target_include_directories(webutil_tests PRIVATE ./include ${PROJECT_ROOT})
target_link_libraries(webutil_tests PRIVATE webutil Boost::unit_test_framework)