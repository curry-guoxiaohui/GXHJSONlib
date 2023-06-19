#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leptjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;



#define TYPE_INT_STRING(intval)\
({\
    char* str;\
    switch(intval)\
    {\
        case LEPT_NULL: str = "LEPT_NULL";break;\
        case LEPT_FALSE: str = "LEPT_FALSE";break;\
        case LEPT_TRUE: str = "LEPT_TRUE";break;\
        case LEPT_NUMBER: str = "LEPT_NUMBER";break;\
        case LEPT_STRING: str = "LEPT_STRING";break;\
        case LEPT_ARRAY: str = "LEPT_ARRAY";break;\
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
    char* str;\
    switch(intval)\
    {\
        case LEPT_PARSE_OK: str = "LEPT_PARSE_OK";break;\
        case LEPT_PARSE_EXPECT_VALUE: str = "LEPT_PARSE_EXPECT_VALUE";break;\
        case LEPT_PARSE_INVALID_VALUE: str = "LEPT_PARSE_INVALID_VALUE";break;\
        case LEPT_PARSE_ROOT_NOT_SINGULAR: str = "LEPT_PARSE_ROOT_NOT_SINGULAR";break;\
        case LEPT_PARSE_NUMBER_TOO_BIG: str = "LEPT_PARSE_NUMBER_TOO_BIG";break;\
        case LEPT_PARSE_MISS_QUOTATION_MARK: str = "LEPT_PARSE_MISS_QUOTATION_MARK";break;\
        case LEPT_PARSE_INVALID_STRING_ESCAPE: str = "LEPT_PARSE_INVALID_STRING_ESCAPE";break;\
        case LEPT_PARSE_INVALID_STRING_CHAR: str = "LEPT_PARSE_INVALID_STRING_CHAR";break;\
        case LEPT_PARSE_INVALID_UNICODE_HEX: str = "LEPT_PARSE_INVALID_UNICODE_HEX";break;\
        case LEPT_PARSE_INVALID_UNICODE_SURROGATE: str = "LEPT_PARSE_INVALID_UNICODE_SURROGATE";break;\
        case LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET: str = "LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET";break;\
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


#define EXPECT_EQ_BASE2(equality,expect,actual,format,str)\
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr,"[error]: %s:%d:expect: " format ",actual: "format "\n",__FILE__,__LINE__,expect,actual);\
            main_ret = 1;\
        }\
        fprintf(stdout,"[info]: %s:%d:parse [%s] expect: " format ",actual: "format "\n",__FILE__,__LINE__,str,expect,actual);\
    }while(0)
#define EXPECT_EQ_DOUBLE(expect, actual,str) EXPECT_EQ_BASE2((expect) == (actual), expect, actual, "%.17g",str)
#define EXPECT_EQ_STRING(expect, actual,alength,str) \
    EXPECT_EQ_BASE2(sizeof(expect) - 1 == alength && memcmp(expect,actual,alength) == 0, expect, actual, "%s",str)
#define EXPECT_TRUE(actual) EXPECT_EQ_BASE2((actual) != 0, "true", "false", "%s","true")
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE2((actual) == 0, "false", "true", "%s","false")
#if defined(_MSC_VER)
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%Iu")
#else
#define EXPECT_EQ_SIZE_T(expect, actual,str) EXPECT_EQ_BASE2((expect) == (actual), (size_t)expect, (size_t)actual, "%zu",str)
#endif

static void test_parse_null()
{
    lept_value v;
    lept_init(&v);
    lept_set_boolean(&v,0);
    const char* p = "null";
    EXPECT_EQ_PARSE(LEPT_PARSE_OK,lept_parse(&v,p),p);
    EXPECT_EQ_GETTYPE(LEPT_NULL,lept_get_type(&v),p);
    lept_free(&v);
}

static void test_parse_true()
{
    lept_value v;
    lept_init(&v);
    lept_set_boolean(&v,0);
    const char* p = "true";
    EXPECT_EQ_PARSE(LEPT_PARSE_OK,lept_parse(&v,p),p);
    EXPECT_EQ_GETTYPE(LEPT_TRUE,lept_get_type(&v),p);
    lept_free(&v);
}

static void test_parse_false()
{
    lept_value v;
    lept_init(&v);
    lept_set_boolean(&v,0);
    const char* p = "false";
    EXPECT_EQ_PARSE(LEPT_PARSE_OK,lept_parse(&v,p),p);
    EXPECT_EQ_GETTYPE(LEPT_FALSE,lept_get_type(&v),p);
    lept_free(&v);
}

#define TEST_NUMBER(expect,json)\
    do{\
        lept_value v;\
        lept_init(&v);\
        EXPECT_EQ_PARSE(LEPT_PARSE_OK,lept_parse(&v,json),json);\
        EXPECT_EQ_GETTYPE(LEPT_NUMBER,lept_get_type(&v),json);\
        EXPECT_EQ_DOUBLE(expect,lept_get_number(&v),json);\
        lept_free(&v);\
    }while(0)

static void test_pase_number()
{
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */

    TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

#define TEST_STRING(expect,json)\
    do{\
        lept_value v;\
        lept_init(&v);\
        EXPECT_EQ_PARSE(LEPT_PARSE_OK,lept_parse(&v,json),json);\
        EXPECT_EQ_GETTYPE(LEPT_STRING,lept_get_type(&v),json);\
        EXPECT_EQ_STRING(expect,lept_get_string(&v),lept_get_string_length(&v),json);\
        lept_free(&v);\
    }while(0)

static void test_parse_string() {
    TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");

    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
    TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}

static void test_parse_array() {
    lept_value v;

    lept_init(&v);
    char* json= "[ ]";
    EXPECT_EQ_PARSE(LEPT_PARSE_OK, lept_parse(&v, json),json);
    EXPECT_EQ_GETTYPE(LEPT_ARRAY, lept_get_type(&v),json);
    EXPECT_EQ_SIZE_T(0, lept_get_array_size(&v),json);

    lept_init(&v);
    json= "[ null , false , true , 123 , \"abc\" ]";
    EXPECT_EQ_PARSE(LEPT_PARSE_OK, lept_parse(&v, json),json);
    EXPECT_EQ_GETTYPE(LEPT_ARRAY, lept_get_type(&v),json);
    EXPECT_EQ_SIZE_T(5, lept_get_array_size(&v),json);
    EXPECT_EQ_GETTYPE(LEPT_NULL, lept_get_type(lept_get_array_element(&v,0)),"arr0");
    EXPECT_EQ_GETTYPE(LEPT_FALSE, lept_get_type(lept_get_array_element(&v,1)),"arr1");
    EXPECT_EQ_GETTYPE(LEPT_TRUE, lept_get_type(lept_get_array_element(&v,2)),"arr2");
    EXPECT_EQ_GETTYPE(LEPT_NUMBER, lept_get_type(lept_get_array_element(&v,3)),"arr3");
    EXPECT_EQ_GETTYPE(LEPT_STRING, lept_get_type(lept_get_array_element(&v,4)),"arr4");
    EXPECT_EQ_DOUBLE(123.0,lept_get_number(lept_get_array_element(&v,3)),"arr3");
    EXPECT_EQ_STRING("abc", lept_get_string(lept_get_array_element(&v, 4)), lept_get_string_length(lept_get_array_element(&v, 4)),"arr4");
    lept_free(&v);

    lept_init(&v);
    json= "[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]";
    EXPECT_EQ_PARSE(LEPT_PARSE_OK, lept_parse(&v, json),json);
    EXPECT_EQ_GETTYPE(LEPT_ARRAY, lept_get_type(&v),json);
    EXPECT_EQ_SIZE_T(4, lept_get_array_size(&v),json);
    for(int i = 0 ; i < 4;i++)
    {
        lept_value* a = lept_get_array_element(&v, i);
        EXPECT_EQ_GETTYPE(LEPT_ARRAY, lept_get_type(a),"arr1");
        EXPECT_EQ_SIZE_T(i, lept_get_array_size(a),"arr2");
        for(int j = 0;j < i;j++)
        {
            lept_value* e = lept_get_array_element(a, j);
            EXPECT_EQ_GETTYPE(LEPT_NUMBER, lept_get_type(e),"arr3");
            EXPECT_EQ_DOUBLE((double)j, lept_get_number(e),"arr4");
        }
    }
    lept_free(&v);
}

#define TEST_ERROR(error,p)\
    do{\
        lept_value v;\
        v.type = LEPT_FALSE;\
        EXPECT_EQ_PARSE(error,lept_parse(&v,p),p);\
        EXPECT_EQ_GETTYPE(LEPT_NULL,lept_get_type(&v),p);\
    }while(0)
static void test_parse_expect_value()
{   
    TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, "");
    TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value()
{
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nul");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "?");


    /* invalid number */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nan");

     /* invalid value in array */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "[1,]");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "[\"a\", nul]");

   
}

static void test_parse_root_not_singular()
{
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "null x");

    
    /* invalid number */
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' , 'E' , 'e' or nothing */
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x123");
}

static void test_parse_number_too_big() {
    TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "-1e309");
}

static void test_parse_missing_quotation_mark() {
    TEST_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK, "\"");
    TEST_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse_invalid_string_escape() {

    TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
    TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
    TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
    TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");

}

static void test_parse_invalid_string_char() {

    TEST_ERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x01\"");
    TEST_ERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x1F\"");

}

static void test_parse_miss_comma_or_square_bracket() {
    TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1");
    TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1}");
    TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1 2");
    TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[[]");
}

static void test_access_null()
{
    lept_value v;
    lept_init(&v);
    lept_set_string(&v,"a",1);
    lept_set_null(&v);
    EXPECT_EQ_GETTYPE(LEPT_NULL,lept_get_type(&v),"null");
    lept_free(&v);
}

static void test_access_boolean()
{
 lept_value v;
    lept_init(&v);
    lept_set_string(&v,"a",1);
    lept_set_boolean(&v,1);
    EXPECT_TRUE(lept_get_boolean(&v));
    lept_set_boolean(&v,0);
    EXPECT_FALSE(lept_get_boolean(&v));
    lept_free(&v);
}

static void test_access_number() {
    lept_value v;
    lept_init(&v);
    lept_set_string(&v, "a", 1);
    lept_set_number(&v, 1234.5);
    EXPECT_EQ_DOUBLE(1234.5, lept_get_number(&v),"1234.5");
    lept_free(&v);
}

static void test_access_string() {
    lept_value v;
    lept_init(&v);
    lept_set_string(&v, "", 0);
    EXPECT_EQ_STRING("", lept_get_string(&v), lept_get_string_length(&v),"");
    lept_set_string(&v, "Hello", 5);
    EXPECT_EQ_STRING("Hello", lept_get_string(&v), lept_get_string_length(&v),"Hello");
    lept_free(&v);
}

static void test_parse_invalid_unicode_hex() {
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u01\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u012\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u/000\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\uG000\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u0G00\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u00/0\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u00G0\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u000/\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u000G\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX, "\"\\u 123\"");
}

static void test_parse_invalid_unicode_surrogate() {
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uDBFF\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\\\\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uDBFF\"");
    TEST_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uE000\"");
}

static void test()
{
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_pase_number();
    test_parse_string();
    test_parse_array();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
    test_parse_number_too_big();
    test_parse_missing_quotation_mark();
    test_parse_invalid_string_escape();
    test_parse_invalid_string_char();
    test_parse_invalid_unicode_hex();
    test_parse_invalid_unicode_surrogate();
    test_parse_miss_comma_or_square_bracket();

    test_access_null();
    test_access_boolean();
    test_access_number();
    test_access_string();
}
int main()
{
    test();
     printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}