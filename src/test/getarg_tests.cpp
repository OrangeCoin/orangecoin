#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-OGC");
    BOOST_CHECK(GetBoolArg("-OGC"));
    BOOST_CHECK(GetBoolArg("-OGC", false));
    BOOST_CHECK(GetBoolArg("-OGC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-OGCo"));
    BOOST_CHECK(!GetBoolArg("-OGCo", false));
    BOOST_CHECK(GetBoolArg("-OGCo", true));

    ResetArgs("-OGC=0");
    BOOST_CHECK(!GetBoolArg("-OGC"));
    BOOST_CHECK(!GetBoolArg("-OGC", false));
    BOOST_CHECK(!GetBoolArg("-OGC", true));

    ResetArgs("-OGC=1");
    BOOST_CHECK(GetBoolArg("-OGC"));
    BOOST_CHECK(GetBoolArg("-OGC", false));
    BOOST_CHECK(GetBoolArg("-OGC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noOGC");
    BOOST_CHECK(!GetBoolArg("-OGC"));
    BOOST_CHECK(!GetBoolArg("-OGC", false));
    BOOST_CHECK(!GetBoolArg("-OGC", true));

    ResetArgs("-noOGC=1");
    BOOST_CHECK(!GetBoolArg("-OGC"));
    BOOST_CHECK(!GetBoolArg("-OGC", false));
    BOOST_CHECK(!GetBoolArg("-OGC", true));

    ResetArgs("-OGC -noOGC");  // -OGC should win
    BOOST_CHECK(GetBoolArg("-OGC"));
    BOOST_CHECK(GetBoolArg("-OGC", false));
    BOOST_CHECK(GetBoolArg("-OGC", true));

    ResetArgs("-OGC=1 -noOGC=1");  // -OGC should win
    BOOST_CHECK(GetBoolArg("-OGC"));
    BOOST_CHECK(GetBoolArg("-OGC", false));
    BOOST_CHECK(GetBoolArg("-OGC", true));

    ResetArgs("-OGC=0 -noOGC=0");  // -OGC should win
    BOOST_CHECK(!GetBoolArg("-OGC"));
    BOOST_CHECK(!GetBoolArg("-OGC", false));
    BOOST_CHECK(!GetBoolArg("-OGC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--OGC=1");
    BOOST_CHECK(GetBoolArg("-OGC"));
    BOOST_CHECK(GetBoolArg("-OGC", false));
    BOOST_CHECK(GetBoolArg("-OGC", true));

    ResetArgs("--noOGC=1");
    BOOST_CHECK(!GetBoolArg("-OGC"));
    BOOST_CHECK(!GetBoolArg("-OGC", false));
    BOOST_CHECK(!GetBoolArg("-OGC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-OGC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OGC", "eleven"), "eleven");

    ResetArgs("-OGC -bar");
    BOOST_CHECK_EQUAL(GetArg("-OGC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OGC", "eleven"), "");

    ResetArgs("-OGC=");
    BOOST_CHECK_EQUAL(GetArg("-OGC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OGC", "eleven"), "");

    ResetArgs("-OGC=11");
    BOOST_CHECK_EQUAL(GetArg("-OGC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-OGC", "eleven"), "11");

    ResetArgs("-OGC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-OGC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-OGC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-OGC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-OGC", 0), 0);

    ResetArgs("-OGC -bar");
    BOOST_CHECK_EQUAL(GetArg("-OGC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-OGC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-OGC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-OGC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-OGC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--OGC");
    BOOST_CHECK_EQUAL(GetBoolArg("-OGC"), true);

    ResetArgs("--OGC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-OGC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noOGC");
    BOOST_CHECK(!GetBoolArg("-OGC"));
    BOOST_CHECK(!GetBoolArg("-OGC", true));
    BOOST_CHECK(!GetBoolArg("-OGC", false));

    ResetArgs("-noOGC=1");
    BOOST_CHECK(!GetBoolArg("-OGC"));
    BOOST_CHECK(!GetBoolArg("-OGC", true));
    BOOST_CHECK(!GetBoolArg("-OGC", false));

    ResetArgs("-noOGC=0");
    BOOST_CHECK(GetBoolArg("-OGC"));
    BOOST_CHECK(GetBoolArg("-OGC", true));
    BOOST_CHECK(GetBoolArg("-OGC", false));

    ResetArgs("-OGC --noOGC");
    BOOST_CHECK(GetBoolArg("-OGC"));

    ResetArgs("-noOGC -OGC"); // OGC always wins:
    BOOST_CHECK(GetBoolArg("-OGC"));
}

BOOST_AUTO_TEST_SUITE_END()
