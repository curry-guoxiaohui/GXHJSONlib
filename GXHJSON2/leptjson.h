#ifndef LEPTJSON_H__
#define LEPTJSON_H__

//JSON的几种数据类型
typedef enum {LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

typedef struct{
    double n;
    lept_type type;
} lept_value;

//解析JSON value的返回值
enum {
    LEPT_PARSE_OK = 0,                  //  解析成功
    LEPT_PARSE_EXPECT_VALUE,            // 一个JSON只含有空白
    LEPT_PARSE_ROOT_NOT_SINGULAR,       // 若一个值之后，在空白之后还有其他字符
    LEPT_PARSE_INVALID_VALUE,           // 若值不是那三种字面量
    LEPT_PARSE_NUMBER_TOO_BIG
};

int lept_parse(lept_value* v,const char* json);

lept_type lept_get_type(const lept_value* v);
double lept_get_number(const lept_value* v);

#endif