#ifndef LEPTJSON_H__
#define LEPTJSON_H__
#include <stddef.h> /*size_t*/
//JSON的几种数据类型
typedef enum {LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;
typedef struct lept_value lept_value;
typedef struct lept_member lept_member;
struct lept_value{
    union{
        double n;                               /*number*/
        struct{                                 /*string:null-terminated string, string length*/
            char* s;
            size_t len;
        }s;
        struct {                                 /* array:  elements, element count */
            lept_value* e;
            size_t size;
        }a;
        struct{                                 /* object: members, member count */
            lept_member* m;
            size_t size;
        }o;
    }u;
    lept_type type;
} ;

struct lept_member{
    char* k;                /*member key string*/
    size_t klen;            /*key string length*/
    lept_value v;            /* member value */
};

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
    LEPT_PARSE_INVALID_UNICODE_HEX,     // \u 后面的十六进制解析错误
    LEPT_PARSE_INVALID_UNICODE_SURROGATE,// 低代理项错误
    LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,//数组的括号不匹配
    LEPT_PARSE_MISS_KEY,                    //解析对象时，都没有key值
    LEPT_PARSE_MISS_COLON,                  //没有value值
    LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET  //括号不匹配
};

//初始化类型为NULL
#define lept_init(v) do{(v)->type = LEPT_NULL;} while(0)

void lept_free(lept_value* v);
#define lept_set_null(v) lept_free(v)

//解析json的入口
int lept_parse(lept_value* v,const char* json);
//将json转化为字符串
char* lept_stringify(const lept_value* v,size_t* length);
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

size_t lept_get_array_size(const lept_value* v);
lept_value* lept_get_array_element(const lept_value* v, size_t index);

size_t lept_get_object_size(const lept_value* v);
const char* lept_get_object_key(const lept_value* v, size_t index);
size_t lept_get_object_key_length(const lept_value* v, size_t index);
lept_value* lept_get_object_value(const lept_value* v, size_t index);
#endif