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
    ResetArgs("-KBC");
    BOOST_CHECK(GetBoolArg("-KBC"));
    BOOST_CHECK(GetBoolArg("-KBC", false));
    BOOST_CHECK(GetBoolArg("-KBC", true));

    BOOST_CHECK(!GetBoolArg("-kb"));
    BOOST_CHECK(!GetBoolArg("-kb", false));
    BOOST_CHECK(GetBoolArg("-kb", true));

    BOOST_CHECK(!GetBoolArg("-KBCk"));
    BOOST_CHECK(!GetBoolArg("-KBCk", false));
    BOOST_CHECK(GetBoolArg("-KBCk", true));

    ResetArgs("-KBC=0");
    BOOST_CHECK(!GetBoolArg("-KBC"));
    BOOST_CHECK(!GetBoolArg("-KBC", false));
    BOOST_CHECK(!GetBoolArg("-KBC", true));

    ResetArgs("-KBC=1");
    BOOST_CHECK(GetBoolArg("-KBC"));
    BOOST_CHECK(GetBoolArg("-KBC", false));
    BOOST_CHECK(GetBoolArg("-KBC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noKBC");
    BOOST_CHECK(!GetBoolArg("-KBC"));
    BOOST_CHECK(!GetBoolArg("-KBC", false));
    BOOST_CHECK(!GetBoolArg("-KBC", true));

    ResetArgs("-noKBC=1");
    BOOST_CHECK(!GetBoolArg("-KBC"));
    BOOST_CHECK(!GetBoolArg("-KBC", false));
    BOOST_CHECK(!GetBoolArg("-KBC", true));

    ResetArgs("-KBC -noKBC");  // -KBC should win
    BOOST_CHECK(GetBoolArg("-KBC"));
    BOOST_CHECK(GetBoolArg("-KBC", false));
    BOOST_CHECK(GetBoolArg("-KBC", true));

    ResetArgs("-KBC=1 -noKBC=1");  // -KBC should win
    BOOST_CHECK(GetBoolArg("-KBC"));
    BOOST_CHECK(GetBoolArg("-KBC", false));
    BOOST_CHECK(GetBoolArg("-KBC", true));

    ResetArgs("-KBC=0 -noKBC=0");  // -KBC should win
    BOOST_CHECK(!GetBoolArg("-KBC"));
    BOOST_CHECK(!GetBoolArg("-KBC", false));
    BOOST_CHECK(!GetBoolArg("-KBC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--KBC=1");
    BOOST_CHECK(GetBoolArg("-KBC"));
    BOOST_CHECK(GetBoolArg("-KBC", false));
    BOOST_CHECK(GetBoolArg("-KBC", true));

    ResetArgs("--noKBC=1");
    BOOST_CHECK(!GetBoolArg("-KBC"));
    BOOST_CHECK(!GetBoolArg("-KBC", false));
    BOOST_CHECK(!GetBoolArg("-KBC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-KBC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-KBC", "eleven"), "eleven");

    ResetArgs("-KBC -kbc");
    BOOST_CHECK_EQUAL(GetArg("-KBC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-KBC", "eleven"), "");

    ResetArgs("-KBC=");
    BOOST_CHECK_EQUAL(GetArg("-KBC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-KBC", "eleven"), "");

    ResetArgs("-KBC=11");
    BOOST_CHECK_EQUAL(GetArg("-KBC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-KBC", "eleven"), "11");

    ResetArgs("-KBC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-KBC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-KBC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-KBC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-KBC", 0), 0);

    ResetArgs("-KBC -kbc");
    BOOST_CHECK_EQUAL(GetArg("-KBC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-kbc", 11), 0);

    ResetArgs("-KBC=11 -kbc=12");
    BOOST_CHECK_EQUAL(GetArg("-KBC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-KBC", 11), 12);

    ResetArgs("-KBC=NaN -kbc=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-KBC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-kbc", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--KBC");
    BOOST_CHECK_EQUAL(GetBoolArg("-KBC"), true);

    ResetArgs("--KBC=verbose --kbc=1");
    BOOST_CHECK_EQUAL(GetArg("-KBC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-KBC", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noKBC");
    BOOST_CHECK(!GetBoolArg("-KBC"));
    BOOST_CHECK(!GetBoolArg("-KBC", true));
    BOOST_CHECK(!GetBoolArg("-KBC", false));

    ResetArgs("-noKBC=1");
    BOOST_CHECK(!GetBoolArg("-KBC"));
    BOOST_CHECK(!GetBoolArg("-KBC", true));
    BOOST_CHECK(!GetBoolArg("-KBC", false));

    ResetArgs("-noKBC=0");
    BOOST_CHECK(GetBoolArg("-KBC"));
    BOOST_CHECK(GetBoolArg("-KBC", true));
    BOOST_CHECK(GetBoolArg("-KBC", false));

    ResetArgs("-KBC --noKBC");
    BOOST_CHECK(GetBoolArg("-KBC"));

    ResetArgs("-noKBC -KBC"); // KBC always wins:
    BOOST_CHECK(GetBoolArg("-KBC"));
}

BOOST_AUTO_TEST_SUITE_END()
