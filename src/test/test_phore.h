#ifndef PHORE_TEST_TEST_PHORE_H
#define PHORE_TEST_TEST_PHORE_H

#include "txdb.h"

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

struct TestingSetup {
    CCoinsViewDB *pcoinsdbview;
    boost::filesystem::path pathTemp;
    boost::thread_group threadGroup;
    ECCVerifyHandle globalVerifyHandle;

    TestingSetup();
    ~TestingSetup();
};

#endif
