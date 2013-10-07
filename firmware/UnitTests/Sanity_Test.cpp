#include <CppUTest/TestHarness.h>

TEST_GROUP(SanityTests)
{
};

TEST(SanityTests, AssertTrueTrue)
{
	CHECK(true);
}
