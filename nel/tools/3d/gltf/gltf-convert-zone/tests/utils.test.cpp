#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <nel/3d/patch.h>

#include <gltf-convert/zone/utils.h>

using ::std::string;
using ::std::vector;

using ::testing::Eq;
using ::testing::StrCaseEq;
using ::testing::IsFalse;
using ::testing::IsNull;
using ::testing::IsTrue;
using ::testing::NotNull;

using ::NL3D::CPatch;

class utilsTest : public testing::Test
{
protected:
	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(utilsTest, zoneName_shouldReturnString)
{
	EXPECT_THAT(zoneName(0 ,0), StrCaseEq("1_AA"));
	EXPECT_THAT(zoneName(1 ,0), StrCaseEq("1_AB"));
	EXPECT_THAT(zoneName(2 ,0), StrCaseEq("1_AC"));
	EXPECT_THAT(zoneName(3 ,0), StrCaseEq("1_AD"));
	EXPECT_THAT(zoneName(0 ,1), StrCaseEq("2_AA"));
	EXPECT_THAT(zoneName(0 ,2), StrCaseEq("3_AA"));
	EXPECT_THAT(zoneName(0 ,3), StrCaseEq("4_AA"));
}
