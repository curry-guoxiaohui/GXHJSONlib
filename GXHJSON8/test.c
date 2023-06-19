#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leptjson.h"
/*wndows 下的CRT检测内存泄漏*/
#ifdef _WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
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
        case LEPT_OBJECT: str = "LEPT_OBJECT";break;\
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
        case LEPT_PARSE_MISS_KEY: str = "LEPT_PARSE_MISS_KEY";break;\
        case LEPT_PARSE_MISS_COLON: str = "LEPT_PARSE_MISS_COLON";break;\
        case LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET: str = "LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET";break;\
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
        if (equality){\
            test_pass++;\
            fprintf(stdout,"[info]: %s:%d:parse [%s] expect: " format ",actual: "format "\n",__FILE__,__LINE__,str,expect,expect);\
        }\
        else {\
            fprintf(stderr,"[error]: %s:%d:expect: " format ",actual: "format "\n",__FILE__,__LINE__,expect,actual);\
            main_ret = 1;\
        }\
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
#define EXPECT_EQ_INT(expect,actual,str) EXPECT_EQ_BASE2((expect) == (actual), expect,actual, "%d",str)


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

static void test_parse_object() {
    lept_value v;

    lept_init(&v);
    char* json= " {\"23\":23,\"fff\":23} ";
    EXPECT_EQ_PARSE(LEPT_PARSE_OK, lept_parse(&v, json),json);
    EXPECT_EQ_GETTYPE(LEPT_OBJECT, lept_get_type(&v),json);
    EXPECT_EQ_SIZE_T(2, lept_get_object_size(&v),json);
    lept_free(&v);

    lept_init(&v);
    json = " { "
        "\"n\" : null , "
        "\"f\" : false , "
        "\"t\" : true , "
        "\"i\" : 123 , "
        "\"s\" : \"abc\", "
        "\"a\" : [ 1, 2, 3 ],"
        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
        " } ";
    EXPECT_EQ_PARSE(LEPT_PARSE_OK, lept_parse(&v, json),json);
    EXPECT_EQ_GETTYPE(LEPT_OBJECT, lept_get_type(&v),json);
    EXPECT_EQ_SIZE_T(7, lept_get_object_size(&v),json); 
    EXPECT_EQ_STRING("n",lept_get_object_key(&v,0),lept_get_object_key_length(&v,0),"obj0");
    EXPECT_EQ_GETTYPE(LEPT_NULL, lept_get_type(lept_get_object_value(&v,0)),"obj0");
    EXPECT_EQ_STRING("f",lept_get_object_key(&v,1),lept_get_object_key_length(&v,1),"obj1");
    EXPECT_EQ_GETTYPE(LEPT_FALSE, lept_get_type(lept_get_object_value(&v,1)),"obj1");
    EXPECT_EQ_STRING("t",lept_get_object_key(&v,2),lept_get_object_key_length(&v,2),"obj2");
    EXPECT_EQ_GETTYPE(LEPT_TRUE, lept_get_type(lept_get_object_value(&v,2)),"obj2");
    EXPECT_EQ_STRING("i",lept_get_object_key(&v,3),lept_get_object_key_length(&v,3),"obj3");
    EXPECT_EQ_GETTYPE(LEPT_NUMBER, lept_get_type(lept_get_object_value(&v,3)),"obj3");
    EXPECT_EQ_STRING("s",lept_get_object_key(&v,4),lept_get_object_key_length(&v,4),"obj4");
    EXPECT_EQ_GETTYPE(LEPT_STRING, lept_get_type(lept_get_object_value(&v,4)),"obj4");
    EXPECT_EQ_STRING("a",lept_get_object_key(&v,5),lept_get_object_key_length(&v,5),"obj5");
    EXPECT_EQ_GETTYPE(LEPT_ARRAY, lept_get_type(lept_get_object_value(&v,5)),"obj5");
    EXPECT_EQ_SIZE_T(3, lept_get_array_size(lept_get_object_value(&v,5)),"obj5");
     for (int i = 0; i < 3; i++) {
        lept_value* e = lept_get_array_element(lept_get_object_value(&v, 5), i);
        EXPECT_EQ_GETTYPE(LEPT_NUMBER, lept_get_type(e),"obji");
        EXPECT_EQ_DOUBLE(i + 1.0, lept_get_number(e),"obji");
    }
    EXPECT_EQ_STRING("o",lept_get_object_key(&v,6),lept_get_object_key_length(&v,6),"obj6");
    {
        lept_value* o = lept_get_object_value(&v, 6);
        EXPECT_EQ_GETTYPE(LEPT_OBJECT, lept_get_type(o),"obj7");
        EXPECT_EQ_SIZE_T(3, lept_get_object_size(o),"obj7"); 
        for (int i = 0; i < 3; i++) {
            lept_value* ov = lept_get_object_value(o, i);
            EXPECT_TRUE('1' + i == lept_get_object_key(o, i)[0]);
            EXPECT_EQ_SIZE_T(1, lept_get_object_key_length(o, i),"obj7");
            EXPECT_EQ_GETTYPE(LEPT_NUMBER, lept_get_type(ov),"obj7");
            EXPECT_EQ_DOUBLE(i + 1.0, lept_get_number(ov),"obj7");
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

static void test_access_array() {
    lept_value a, e;
    size_t i, j;

    lept_init(&a);

    for (j = 0; j <= 5; j += 5) {
        lept_set_array(&a, j);
        EXPECT_EQ_SIZE_T(0, lept_get_array_size(&a),"init array");
        EXPECT_EQ_SIZE_T(j, lept_get_array_capacity(&a),"get array capacity");
        for (i = 0; i < 10; i++) {
            lept_init(&e);
            lept_set_number(&e, i);
            lept_move(lept_pushback_array_element(&a), &e);
            lept_free(&e);
        }

        EXPECT_EQ_SIZE_T(10, lept_get_array_size(&a),"get array size");
        for (i = 0; i < 10; i++)
            EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)),"get array number");
    }

    lept_popback_array_element(&a);
    EXPECT_EQ_SIZE_T(9, lept_get_array_size(&a),"pop array");
    for (i = 0; i < 9; i++)
        EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)),"get array number");

    lept_erase_array_element(&a, 4, 0);
    EXPECT_EQ_SIZE_T(9, lept_get_array_size(&a),"erase array 0 element");
    for (i = 0; i < 9; i++)
        EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)),"get array number");

    lept_erase_array_element(&a, 8, 1);
    EXPECT_EQ_SIZE_T(8, lept_get_array_size(&a),"erase array 1 element");
    for (i = 0; i < 8; i++)
        EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)),"get array number");

    lept_erase_array_element(&a, 0, 2);
    EXPECT_EQ_SIZE_T(6, lept_get_array_size(&a),"erase array 2 element");
    for (i = 0; i < 6; i++)
        EXPECT_EQ_DOUBLE((double)i + 2, lept_get_number(lept_get_array_element(&a, i)),"get array number");


    for (i = 0; i < 2; i++) {
        lept_init(&e);
        lept_set_number(&e, i);
        lept_move(lept_insert_array_element(&a, i), &e);
        lept_free(&e);
    }
    
    EXPECT_EQ_SIZE_T(8, lept_get_array_size(&a),"array insert 2 element");
    for (i = 0; i < 8; i++)
        EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)),"get array number");

    EXPECT_TRUE(lept_get_array_capacity(&a) > 8);
    lept_shrink_array(&a);
    EXPECT_EQ_SIZE_T(8, lept_get_array_capacity(&a),"capacity after shrink");
    EXPECT_EQ_SIZE_T(8, lept_get_array_size(&a),"size after shrink");
    for (i = 0; i < 8; i++)
        EXPECT_EQ_DOUBLE((double)i, lept_get_number(lept_get_array_element(&a, i)),"get array number");

    lept_set_string(&e, "Hello", 5);
    lept_move(lept_pushback_array_element(&a), &e);     /* Test if element is freed */
    lept_free(&e);

    i = lept_get_array_capacity(&a);
    lept_clear_array(&a);
    EXPECT_EQ_SIZE_T(0, lept_get_array_size(&a),"size after clear");
    EXPECT_EQ_SIZE_T(i, lept_get_array_capacity(&a),"capacity after clear");   /* capacity remains unchanged */
    lept_shrink_array(&a);
    EXPECT_EQ_SIZE_T(0, lept_get_array_capacity(&a),"capacity after shrink");

    lept_free(&a);
}

static void test_access_object() {
    lept_value o, v, *pv;
    size_t i, j, index;

    lept_init(&o);

    for (j = 0; j <= 5; j += 5) {
        lept_set_object(&o, j);
        EXPECT_EQ_SIZE_T(0, lept_get_object_size(&o),"size after init");
        EXPECT_EQ_SIZE_T(j, lept_get_object_capacity(&o),"capacity after init");
        for (i = 0; i < 10; i++) {
            char key[2] = "a";
            key[0] += i;
            lept_init(&v);
            lept_set_number(&v, i);
            lept_move(lept_set_object_value(&o, key, 1), &v);
            lept_free(&v);
        }
        EXPECT_EQ_SIZE_T(10, lept_get_object_size(&o),"size after set");
        for (i = 0; i < 10; i++) {
            char key[] = "a";
            key[0] += i;
            index = lept_find_object_index(&o, key, 1);
            EXPECT_TRUE(index != LEPT_KEY_NOT_EXIST);
            pv = lept_get_object_value(&o, index);
            EXPECT_EQ_DOUBLE((double)i, lept_get_number(pv),"get value of key");
        }
    }

    index = lept_find_object_index(&o, "j", 1);    
    EXPECT_TRUE(index != LEPT_KEY_NOT_EXIST);
    lept_remove_object_value(&o, index);
    index = lept_find_object_index(&o, "j", 1);
    EXPECT_TRUE(index == LEPT_KEY_NOT_EXIST);
    EXPECT_EQ_SIZE_T(9, lept_get_object_size(&o),"size after remove j");

    index = lept_find_object_index(&o, "a", 1);
    EXPECT_TRUE(index != LEPT_KEY_NOT_EXIST);
    lept_remove_object_value(&o, index);
    index = lept_find_object_index(&o, "a", 1);
    EXPECT_TRUE(index == LEPT_KEY_NOT_EXIST);
    EXPECT_EQ_SIZE_T(8, lept_get_object_size(&o),"size after remove a");

    EXPECT_TRUE(lept_get_object_capacity(&o) > 8);
    lept_shrink_object(&o);
    EXPECT_EQ_SIZE_T(8, lept_get_object_capacity(&o),"capacity after shrink");
    EXPECT_EQ_SIZE_T(8, lept_get_object_size(&o),"size after shrink");
    for (i = 0; i < 8; i++) {
        char key[] = "a";
        key[0] += i + 1;
        EXPECT_EQ_DOUBLE((double)i + 1, lept_get_number(lept_get_object_value(&o, lept_find_object_index(&o, key, 1))),"object get value of key");
    }

    lept_set_string(&v, "Hello", 5);
    lept_move(lept_set_object_value(&o, "World", 5), &v); /* Test if element is freed */
    lept_free(&v);

    pv = lept_find_object_value(&o, "World", 5);
    EXPECT_TRUE(pv != NULL);
    EXPECT_EQ_STRING("Hello", lept_get_string(pv), lept_get_string_length(pv),"get value of key World");

    i = lept_get_object_capacity(&o);
    lept_clear_object(&o);
    EXPECT_EQ_SIZE_T(0, lept_get_object_size(&o),"size after clear");
    EXPECT_EQ_SIZE_T(i, lept_get_object_capacity(&o),"capacity after clear"); /* capacity remains unchanged */
    lept_shrink_object(&o);
    EXPECT_EQ_SIZE_T(0, lept_get_object_capacity(&o),"capacity after shrink");

    lept_free(&o);
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

static void test_parse_miss_key() {
    TEST_ERROR(LEPT_PARSE_MISS_KEY, "{:1,");
    TEST_ERROR(LEPT_PARSE_MISS_KEY, "{1:1,");
    TEST_ERROR(LEPT_PARSE_MISS_KEY, "{true:1,");
    TEST_ERROR(LEPT_PARSE_MISS_KEY, "{false:1,");
    TEST_ERROR(LEPT_PARSE_MISS_KEY, "{null:1,");
    TEST_ERROR(LEPT_PARSE_MISS_KEY, "{[]:1,");
    TEST_ERROR(LEPT_PARSE_MISS_KEY, "{{}:1,");
    TEST_ERROR(LEPT_PARSE_MISS_KEY, "{\"a\":1,");
}

static void test_parse_miss_colon() {
    TEST_ERROR(LEPT_PARSE_MISS_COLON, "{\"a\"}");
    TEST_ERROR(LEPT_PARSE_MISS_COLON, "{\"a\",\"b\"}");
}

static void test_parse_miss_comma_or_curly_bracket() {
    TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1");
    TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1]");
    TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1 \"b\"");
    TEST_ERROR(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":{}");
}

static void test_parse()
{
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_pase_number();
    test_parse_string();
    test_parse_array();
    test_parse_object();
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
    test_parse_miss_key();
    test_parse_miss_colon();
    test_parse_miss_comma_or_curly_bracket();

  
}

static void test_access()
{
   test_access_null();
    test_access_boolean();
    test_access_number();
    test_access_string();
    test_access_array();
    test_access_object();
    
}

#define TEST_ROUNDTRIP(json)\
    do{\
        lept_value v;\
        char* json2;\
        size_t length;\
        lept_init(&v);\
        EXPECT_EQ_PARSE(LEPT_PARSE_OK,lept_parse(&v,json),json);\
        json2 = lept_stringify(&v,&length);\
        EXPECT_EQ_STRING(json,json2,length,json);\
        lept_free(&v);\
        free(json2);\
    }while(0)

static void test_stringify_number() {
    TEST_ROUNDTRIP("0");
    TEST_ROUNDTRIP("-0");
    TEST_ROUNDTRIP("1");
    TEST_ROUNDTRIP("-1");
    TEST_ROUNDTRIP("1.5");
    TEST_ROUNDTRIP("-1.5");
    TEST_ROUNDTRIP("3.25");
    TEST_ROUNDTRIP("1e+20");
    TEST_ROUNDTRIP("1.234e+20");
    TEST_ROUNDTRIP("1.234e-20");

    TEST_ROUNDTRIP("1.0000000000000002"); /* the smallest number > 1 */
    TEST_ROUNDTRIP("4.9406564584124654e-324"); /* minimum denormal */
    TEST_ROUNDTRIP("-4.9406564584124654e-324");
    TEST_ROUNDTRIP("2.2250738585072009e-308");  /* Max subnormal double */
    TEST_ROUNDTRIP("-2.2250738585072009e-308");
    TEST_ROUNDTRIP("2.2250738585072014e-308");  /* Min normal positive double */
    TEST_ROUNDTRIP("-2.2250738585072014e-308");
    TEST_ROUNDTRIP("1.7976931348623157e+308");  /* Max double */
    TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

static void test_stringify_string() {
    TEST_ROUNDTRIP("\"\"");
    TEST_ROUNDTRIP("\"Hello\"");
    TEST_ROUNDTRIP("\"Hello\\nWorld\"");
    TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

static void test_stringify_array() {
    TEST_ROUNDTRIP("[]");
    TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

static void test_stringify_object() {
    TEST_ROUNDTRIP("{}");
    TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}

static void test_stringify()
{
    TEST_ROUNDTRIP("null");
    TEST_ROUNDTRIP("true");
    TEST_ROUNDTRIP("false");
    test_stringify_number();
    test_stringify_string();
     test_stringify_array();
    test_stringify_object();
}

#define TEST_EQUAL(json1,json2,equality)\
    do{\
        lept_value v1,v2;\
        lept_init(&v1);\
        lept_init(&v2);\
        EXPECT_EQ_PARSE(LEPT_PARSE_OK,lept_parse(&v1,json1),json1);\
        EXPECT_EQ_PARSE(LEPT_PARSE_OK,lept_parse(&v2,json2),json2);\
        EXPECT_EQ_INT(equality,lept_is_equal(&v1,&v2),"json1 ?= json2");\
        lept_free(&v1);\
        lept_free(&v2);\
    }while(0)

static void test_equal() {
    TEST_EQUAL("true", "true", 1);
    TEST_EQUAL("true", "false", 0);
    TEST_EQUAL("false", "false", 1);
    TEST_EQUAL("null", "null", 1);
    TEST_EQUAL("null", "0", 0);
    TEST_EQUAL("123", "123", 1);
    TEST_EQUAL("123", "456", 0);
    TEST_EQUAL("\"abc\"", "\"abc\"", 1);
    TEST_EQUAL("\"abc\"", "\"abcd\"", 0);
    TEST_EQUAL("[]", "[]", 1);
    TEST_EQUAL("[]", "null", 0);
    TEST_EQUAL("[1,2,3]", "[1,2,3]", 1);
    TEST_EQUAL("[1,2,3]", "[1,2,3,4]", 0);
    TEST_EQUAL("[[]]", "[[]]", 1);
    TEST_EQUAL("{}", "{}", 1);
    TEST_EQUAL("{}", "null", 0);
    TEST_EQUAL("{}", "[]", 0);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2}", 1);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"b\":2,\"a\":1}", 1);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":3}", 0);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2,\"c\":3}", 0);
    TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":{}}}}", 1);
    TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":[]}}}", 0);
}

static void test_copy()
{
    lept_value v1,v2;
    lept_init(&v1);
    lept_parse(&v1, "{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
    lept_init(&v2);
    lept_copy(&v2,&v1);
    EXPECT_TRUE(lept_is_equal(&v1,&v2));
    lept_free(&v1);
    lept_free(&v2);
}

static void test_move() {
    lept_value v1, v2, v3;
    lept_init(&v1);
    lept_parse(&v1, "{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
    lept_init(&v2);
    lept_copy(&v2, &v1);
    lept_init(&v3);
    lept_move(&v3, &v2);
    EXPECT_EQ_PARSE(LEPT_NULL, lept_get_type(&v2),"test_move");
    EXPECT_TRUE(lept_is_equal(&v3, &v1));
    lept_free(&v1);
    lept_free(&v2);
    lept_free(&v3);
}

static void test_swap() {
    lept_value v1, v2;
    lept_init(&v1);
    lept_init(&v2);
    lept_set_string(&v1, "Hello",  5);
    lept_set_string(&v2, "World!", 6);
    lept_swap(&v1, &v2);
    EXPECT_EQ_STRING("World!", lept_get_string(&v1), lept_get_string_length(&v1),"test_swap(src:Hello)");
    EXPECT_EQ_STRING("Hello",  lept_get_string(&v2), lept_get_string_length(&v2),"test_swap(src:World)");
    lept_free(&v1);
    lept_free(&v2);
}
static void test_other()
{
    test_equal();
    test_copy();
    test_move();
    test_swap();
}

int main()
{
    #ifdef _WINDOWS
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif
    test_parse();
    test_stringify();
    test_other();
    test_access();
     printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}