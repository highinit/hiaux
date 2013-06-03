
class stubgen_test : public ::testing::Test 
{
	protected:
	
	stubgen_t *stubgen;

	virtual void SetUp() 
	{
/*	stubgen = new stubgen_t();
	stubgen->gen("hfss_t", "hfss.h", "skelhfs.h");
*/	} 

};

void printall(pair<char*, int> *p)
{
	for (int i = 0; i<p->second; i++)
		printf("%c", p->first[i]);
}
/*
TEST_F(stubgen_test, ExtractTypes) 
{
	//vecpair_t *f = stubgen->get_type("read_ret_t");
	//printf("!!! %s !!!", f->at(0).second);
	//EXPECT_EQ(0, strcmp(f->at(0).first,"vector<bar>") );
	//EXPECT_EQ(0, strcmp(f->at(0).second,"a ") );
	
//	EXPECT_EQ(0, strcmp(f->at(1).first,"int") );
//	EXPECT_EQ(0, strcmp(f->at(1).second,"crc ") );
	type_t *type = stubgen->type->at(5);
//	repr_gen::init();
//	printall(repr_gen::gen_all(type));
}
*/