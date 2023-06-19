#ifndef LEPTJSON_H__
#define LEPTJSON_H__
#include <stddef.h> /*size_t*/
//JSON的几种数据类型
typedef enum {LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

typedef struct{
    union{
        double n;
        struct{
            char* s;
            size_t len;
        }s;
    }u;
    lept_type type;
} lept_value;

//解析JSON value的返回值
enum {
    LEPT_PARSE_OK = 0,                  //  解析成功
    LEPT_PARSE_EXPECT_VALUE,            // 一个JSON只含有空白
    LEPT_PARSE_ROOT_NOT_SINGULAR,       // 若一个值之后，在空白之后还有其他字符
    LEPT_PARSE_INVALID_VALUE,           // 若值不是那三种字面量
    LEPT_PARSE_NUMBER_TOO_BIG,          //解析的数字超过double的范围
    LEPT_PARSE_MISS_QUOTATION_MARK,     //只有前引号
    LEPT_PARSE_INVALID_STRING_ESCAPE,   //转义字符不存在
    LEPT_PARSE_INVALID_STRING_CHAR,     //不合法的char值
    LEPT_PARSE_INVALID_UNICODE_HEX,
    LEPT_PARSE_INVALID_UNICODE_SURROGATE
};

//初始化类型为NULL
#define lept_init(v) do{(v)->type = LEPT_NULL;} while(0)

void lept_free(lept_value* v);
#define lept_set_null(v) lept_free(v)

//解析json的入口
int lept_parse(lept_value* v,const char* json);
//获取解析后的类型
lept_type lept_get_type(const lept_value* v);

int lept_get_boolean(const lept_value* v);
void lept_set_boolean(lept_value*v,int b);

//获取解析number类型的数值
double lept_get_number(const lept_value* v);
void lept_set_number(lept_value* v,double n);

const char* lept_get_string(const lept_value* v);
size_t lept_get_string_length(const lept_value* v);
void lept_set_string(lept_value* v,const char* s,size_t len);
#endif