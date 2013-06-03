

TEST(instring, all)
{
   	::testing::FLAGS_gtest_death_test_style = "threadsafe";
    EXPECT_EQ (1, instring("eg he", "sieg heil"));
    EXPECT_EQ (1, instring("hitler", "hitler"));
    EXPECT_EQ (1, instring("hit", "hitler"));
	EXPECT_EQ (1, instring("ler", "hitler"));
	EXPECT_EQ (0, instring("hitlera", "hitler"));
	EXPECT_EQ (0, instring("white", "power"));

	EXPECT_EQ (1, !strcmp(inside_vec("vector<nigga>"), "nigga") );
	EXPECT_EQ (1, !strcmp(inside_vec(" vector <hitler>"), "hitler") );

	//printf("!!!!!!!!!!!!!! %s", inside_vec("vector<nigga>"));
}

TEST(firststr, all)
{
   	::testing::FLAGS_gtest_death_test_style = "threadsafe";
    EXPECT_EQ (1, instring("sieg", "sieg heil"));
	EXPECT_EQ (0, instring("white", "nigga"));
}


