#include <boost/test/unit_test.hpp>

#include "webutil/PkceManager.h"

BOOST_AUTO_TEST_SUITE(class_PkceManager)
    
    BOOST_AUTO_TEST_CASE(function_get_code_verifier) {
        webutil::PKCE manager;
        BOOST_CHECK_EQUAL(manager.get_code_verifier().length(), 44); // 44 characters from encoding 128 bits
    }


BOOST_AUTO_TEST_SUITE_END();