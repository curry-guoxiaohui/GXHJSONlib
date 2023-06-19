#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leptjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define TYPE_INT_STRING(intval)\
({\
    register char* str;\  
    switch(intval)\
    {\
        case LEPT_NULL: str = "LEPT_NULL";break;\
        case LEPT_FALSE: str = "LEPT_FALSE";break;\
        case LEPT_TRUE: str = "LEPT_PARSE_OK";break;\
        default:str = "";\
    }\
str;})
#define EXPECT_EQ_BASE(equality,expect,actual,format,str)\
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr,"[error]: %s:%d:expect: " format ",actual:"format "\n",__FILE__,__LINE__,expect,actual);\
            main_ret = 1;\
        }\
        fprintf(stdout,"[info]: %s:%d:parse [%s] expect: %s ,actual: %s \n",__FILE__,__LINE__,str,TYPE_INT_STRING(expect),TYPE_INT_STRING(actual));\
    }while(0)
#define EXPECT_EQ_GETTYPE(expect, actual,str) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d",str)


#define RETURN_INT_STRING(intval)\
({\
    register char* str;\  
    switch(intval)\
    {\
        case LEPT_PARSE_OK: str = "LEPT_PARSE_OK";break;\
        case LEPT_PARSE_EXPECT_VALUE: str = "LEPT_PARSE_OK";break;\
        case LEPT_PARSE_INVALID_VALUE: str = "LEPT_PARSE_OK";break;\
        case LEPT_PARSE_ROOT_NOT_SINGULAR: str = "LEPT_PARSE_ROOT_NOT_SINGULAR";break;\
        default:str = "";\
    }\
str;})
#define EXPECT_EQ_BASE1(equality,expect,actual,format,str)\
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr,"[error]: %s:%d:expect: " format ",actual:"format "\n",__FILE__,__LINE__,expect,actual);\
            main_ret = 1;\
        }\
        fprintf(stdout,"[info]: %s:%d:parse [%s] expect: %s ,actual: %s \n",__FILE__,__LINE__,str,RETURN_INT_STRING(expect),RETURN_INT_STRING(actual));\
    }while(0)
#define EXPECT_EQ_PARSE(expect, actual,str) EXPECT_EQ_BASE1((expect) == (actual), expect, actual, "%d",str)

static void test_parse_null()
{
    lept_value v;
    v.type = LEPT_FALSE;
    const char* p = "null";
    EXPECT_EQ_PARSE(LEPT_PARSE_OK,lept_parse(&v,p),p);
    EXPECT_EQ_GETTYPE(LEPT_NULL,lept_get_type(&v),p);
}

static void test_parse_true()
{
    lept_value v;
    v.type = LEPT_FALSE;
    const char* p = "true";
    EXPECT_EQ_PARSE(LEPT_PARSE_OK,lept_parse(&v,p),p);
    EXPECT_EQ_GETTYPE(LEPT_TRUE,lept_get_type(&v),p);
}

static void test_parse_false()
{
    lept_value v;
    v.type = LEPT_TRUE;
    const char* p = "false";
    EXPECT_EQ_PARSE(LEPT_PARSE_OK,lept_parse(&v,p),p);
    EXPECT_EQ_GETTYPE(LEPT_FALSE,lept_get_type(&v),p);
}

static void test_parse_expect_value()
{
    lept_value v;
    v.type = LEPT_FALSE;
    const char* p = "";
    EXPECT_EQ_PARSE(LEPT_PARSE_EXPECT_VALUE,lept_parse(&v,p),p);
    EXPECT_EQ_GETTYPE(LEPT_NULL,lept_get_type(&v),p);

    v.type = LEPT_FALSE;
    p = " ";
    EXPECT_EQ_PARSE(LEPT_PARSE_EXPECT_VALUE,lept_parse(&v,p),p);
    EXPECT_EQ_GETTYPE(LEPT_NULL,lept_get_type(&v),p);
}

static void test_parse_invalid_value()
{
    lept_value v;
    v.type = LEPT_FALSE;
    const char* p = "nul";
    EXPECT_EQ_PARSE(LEPT_PARSE_INVALID_VALUE,lept_parse(&v,p),p);
    EXPECT_EQ_GETTYPE(LEPT_NULL,lept_get_type(&v),p);

    v.type = LEPT_FALSE;
    p = "?";
    EXPECT_EQ_PARSE(LEPT_PARSE_INVALID_VALUE,lept_parse(&v,p),p);
    EXPECT_EQ_GETTYPE(LEPT_NULL,lept_get_type(&v),p);
}

static void test_parse_root_not_singular()
{
    lept_value v;
    v.type = LEPT_FALSE;
    const char* p = "null x";
    EXPECT_EQ_PARSE(LEPT_PARSE_ROOT_NOT_SINGULAR,lept_parse(&v,p),p);
    EXPECT_EQ_GETTYPE(LEPT_NULL,lept_get_type(&v),p);

}
static void test()
{
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}
int main()
{
    test();
     printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}