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
    ResetArgs("-SPC");
    BOOST_CHECK(GetBoolArg("-SPC"));
    BOOST_CHECK(GetBoolArg("-SPC", false));
    BOOST_CHECK(GetBoolArg("-SPC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-SPCo"));
    BOOST_CHECK(!GetBoolArg("-SPCo", false));
    BOOST_CHECK(GetBoolArg("-SPCo", true));

    ResetArgs("-SPC=0");
    BOOST_CHECK(!GetBoolArg("-SPC"));
    BOOST_CHECK(!GetBoolArg("-SPC", false));
    BOOST_CHECK(!GetBoolArg("-SPC", true));

    ResetArgs("-SPC=1");
    BOOST_CHECK(GetBoolArg("-SPC"));
    BOOST_CHECK(GetBoolArg("-SPC", false));
    BOOST_CHECK(GetBoolArg("-SPC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noSPC");
    BOOST_CHECK(!GetBoolArg("-SPC"));
    BOOST_CHECK(!GetBoolArg("-SPC", false));
    BOOST_CHECK(!GetBoolArg("-SPC", true));

    ResetArgs("-noSPC=1");
    BOOST_CHECK(!GetBoolArg("-SPC"));
    BOOST_CHECK(!GetBoolArg("-SPC", false));
    BOOST_CHECK(!GetBoolArg("-SPC", true));

    ResetArgs("-SPC -noSPC");  // -SPC should win
    BOOST_CHECK(GetBoolArg("-SPC"));
    BOOST_CHECK(GetBoolArg("-SPC", false));
    BOOST_CHECK(GetBoolArg("-SPC", true));

    ResetArgs("-SPC=1 -noSPC=1");  // -SPC should win
    BOOST_CHECK(GetBoolArg("-SPC"));
    BOOST_CHECK(GetBoolArg("-SPC", false));
    BOOST_CHECK(GetBoolArg("-SPC", true));

    ResetArgs("-SPC=0 -noSPC=0");  // -SPC should win
    BOOST_CHECK(!GetBoolArg("-SPC"));
    BOOST_CHECK(!GetBoolArg("-SPC", false));
    BOOST_CHECK(!GetBoolArg("-SPC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--SPC=1");
    BOOST_CHECK(GetBoolArg("-SPC"));
    BOOST_CHECK(GetBoolArg("-SPC", false));
    BOOST_CHECK(GetBoolArg("-SPC", true));

    ResetArgs("--noSPC=1");
    BOOST_CHECK(!GetBoolArg("-SPC"));
    BOOST_CHECK(!GetBoolArg("-SPC", false));
    BOOST_CHECK(!GetBoolArg("-SPC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-SPC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-SPC", "eleven"), "eleven");

    ResetArgs("-SPC -bar");
    BOOST_CHECK_EQUAL(GetArg("-SPC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-SPC", "eleven"), "");

    ResetArgs("-SPC=");
    BOOST_CHECK_EQUAL(GetArg("-SPC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-SPC", "eleven"), "");

    ResetArgs("-SPC=11");
    BOOST_CHECK_EQUAL(GetArg("-SPC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-SPC", "eleven"), "11");

    ResetArgs("-SPC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-SPC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-SPC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-SPC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-SPC", 0), 0);

    ResetArgs("-SPC -bar");
    BOOST_CHECK_EQUAL(GetArg("-SPC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-SPC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-SPC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-SPC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-SPC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--SPC");
    BOOST_CHECK_EQUAL(GetBoolArg("-SPC"), true);

    ResetArgs("--SPC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-SPC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noSPC");
    BOOST_CHECK(!GetBoolArg("-SPC"));
    BOOST_CHECK(!GetBoolArg("-SPC", true));
    BOOST_CHECK(!GetBoolArg("-SPC", false));

    ResetArgs("-noSPC=1");
    BOOST_CHECK(!GetBoolArg("-SPC"));
    BOOST_CHECK(!GetBoolArg("-SPC", true));
    BOOST_CHECK(!GetBoolArg("-SPC", false));

    ResetArgs("-noSPC=0");
    BOOST_CHECK(GetBoolArg("-SPC"));
    BOOST_CHECK(GetBoolArg("-SPC", true));
    BOOST_CHECK(GetBoolArg("-SPC", false));

    ResetArgs("-SPC --noSPC");
    BOOST_CHECK(GetBoolArg("-SPC"));

    ResetArgs("-noSPC -SPC"); // SPC always wins:
    BOOST_CHECK(GetBoolArg("-SPC"));
}

BOOST_AUTO_TEST_SUITE_END()
