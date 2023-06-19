#include "leptjson.h"
#include <assert.h> /*assert()*/
#include <stdlib.h> /*NULL strtod() malloc() realloc() free()*/
#include <math.h> /*HUGE_VAL*/
#include <errno.h> /*errno ERANGE 表示一个范围错误，它在输入参数超出数学函数定义的范围时发生，errno 被设置为 ERANGE。*/
#include <string.h> /*memcpy()*/
#include <stdio.h>

/*将lept_context作为输入缓冲区的默认初始化大小*/
#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256
#endif
/*将lept_context作为输出缓冲区的默认初始化大小*/
#ifndef LEPT_PARSE_STRINGIFY_INIT_SIZE
#define LEPT_PARSE_STRINGIFY_INIT_SIZE 256
#endif

#define EXPECT(c,ch) do{ assert(*c->json == (ch));c->json++; }while(0)
#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')
#define PUTC(c, ch)         do { *(char*)lept_context_push(c, sizeof(char)) = (ch); } while(0)
#define PUTS(c,s,len)    memcpy(lept_context_push(c,len),s,len)

typedef struct {
    const char* json;
    char* stack;
    size_t size,top;
}lept_context;

static void lept_parse_whitespace(lept_context* c)
{
    const char* p = c->json;
    while(*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
    {
        p++;
    }
    c->json = p;
}

static int lept_parse_literal(lept_context* c,lept_value* v,const char* literal,lept_type type)
{
    EXPECT(c,literal[0]);
    size_t i;
    for(i = 0; literal[i + 1];i++)
        if(c->json[i] != literal[i + 1])
         return LEPT_PARSE_INVALID_VALUE;
    c->json += i;
    v->type = type;
    return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c, lept_value* v) {
    const char* p = c->json;
    if (*p == '-') p++;
    if (*p == '0') p++;
    else {
        if (!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    if (*p == '.') {
        p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '+' || *p == '-') p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    errno = 0;
    v->u.n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
        return LEPT_PARSE_NUMBER_TOO_BIG;
    v->type = LEPT_NUMBER;
    c->json = p;
    return LEPT_PARSE_OK;
}

static void* lept_context_push(lept_context* c, size_t size) {
    void* ret;
    assert(size > 0);
    if (c->top + size >= c->size) {
        if (c->size == 0)
            c->size = LEPT_PARSE_STACK_INIT_SIZE;
        while (c->top + size >= c->size)
            c->size += c->size >> 1;  /* c->size * 1.5 */
        c->stack = (char*)realloc(c->stack, c->size);
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;
}

static void* lept_context_pop(lept_context* c, size_t size) {
    assert(c->top >= size);
    return c->stack + (c->top -= size);
}

/*解析十六进制*/
static const char* lept_parse_hex4(const char* p, unsigned* u) {
    int i = 0;
    *u = 0;
    for(int i = 0; i < 4; i++)
    {
        char ch = *p++;
        *u <<= 4;
        if (ch >= '0' && ch <= '9') *u |= ch - '0';
        else if(ch >= 'A' && ch <= 'F') *u |= ch - ('A' - 10); //因为A 代表10 ，所以减去之后需要再加10
        else if(ch >= 'a' && ch <= 'f') *u |= ch - ('a' - 10);
        else return NULL; 
        
    }
    return p;
}

/*将码点转化位utf-8编码*/
static void lept_encode_utf8(lept_context* c, unsigned u) {
    if(u <= 0x7F)
        PUTC(c,u & 0xFF);
    else if(u <= 0x7FF)
    {
        PUTC(c, 0xC0 | ((u >> 6) & 0xFF));//0xFF表示取剩余的所有数据 ,| 0XC0是指最高位添加11
        PUTC(c, 0x80 | ( u       & 0x3F));//0x3F表示只取低6位的数据 ,| 0X80是指最高位添加10
    }
    else if (u <= 0xFFFF) {
        PUTC(c, 0xE0 | ((u >> 12) & 0xFF));
        PUTC(c, 0x80 | ((u >>  6) & 0x3F));
        PUTC(c, 0x80 | ( u        & 0x3F));
    }
    else {
        assert(u <= 0x10FFFF);
        PUTC(c, 0xF0 | ((u >> 18) & 0xFF));
        PUTC(c, 0x80 | ((u >> 12) & 0x3F));
        PUTC(c, 0x80 | ((u >>  6) & 0x3F));
        PUTC(c, 0x80 | ( u        & 0x3F));
    }
}

#define STRING_ERROR(ret) do { c->top = head; return ret; } while(0)

static int lept_parse_string_raw(lept_context* c,char** str,size_t* len)
{
    size_t head = c->top;
    unsigned u,u2;
    const char* p;
    EXPECT(c, '\"');
    p = c->json;
    for (;;) {
        char ch = *p++;
        switch (ch) {
            case '\"':
                *len = c->top - head;
                *str = lept_context_pop(c,*len);
                c->json = p;
                return LEPT_PARSE_OK;
            case '\\':
                switch(*p++){
                    case '\"':PUTC(c,'\"');break;
                    case '\\':PUTC(c,'\\');break;
                    case '/':PUTC(c,'/');break;
                    case 'b':PUTC(c,'\b');break;
                    case 'f':PUTC(c,'\f');break;
                    case 'n':PUTC(c,'\n');break;
                    case 'r':PUTC(c,'\r');break;
                    case 't':PUTC(c,'\t');break;
                    case 'u':
                        if(!(p = lept_parse_hex4(p,&u)))
                            STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
                        /*处理代理对*/
                        /*遇到高代理项，就需要把低代理项 \uxxxx 也解析进来，然后用这两个项去计算出码点：*/
                        if (u >= 0xD800 && u <= 0xDBFF) { /* surrogate pair */
                            if (*p++ != '\\')
                                STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
                            if (*p++ != 'u')
                                STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
                            if (!(p = lept_parse_hex4(p, &u2)))
                                STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
                            if (u2 < 0xDC00 || u2 > 0xDFFF)
                                STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_SURROGATE);
                            u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                         }
                        lept_encode_utf8(c,u);
                        break;
                    default:
                        STRING_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE);
                }
                break;
            case '\0':
                STRING_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK);
            default:
                if((unsigned char)ch < 0x20){
                    STRING_ERROR(LEPT_PARSE_INVALID_STRING_CHAR);
                }
                PUTC(c, ch);
        }
    }
}
static int lept_parse_string(lept_context* c, lept_value* v) {
    int ret;
    char* s;
    size_t len;
    if((ret = lept_parse_string_raw(c,&s,&len)) == LEPT_PARSE_OK)
        lept_set_string(v, s, len);
    return ret;
}

static int lept_parse_value(lept_context* c, lept_value* v);

static int lept_parse_array(lept_context* c, lept_value* v) {
    size_t size = 0;
    int ret = 0;
    EXPECT(c,'[');
    lept_parse_whitespace(c);
    if(*c->json == ']')
    {
        c->json++;
        lept_set_array(v, 0);
        return LEPT_PARSE_OK;
    }

    for(;;)
    {
        lept_value e;
        lept_init(&e);
        if((ret = lept_parse_value(c,&e)) != LEPT_PARSE_OK)
        {
            break;
        }
        memcpy(lept_context_push(c,sizeof(lept_value)),&e,sizeof(lept_value));
        size++;
        lept_parse_whitespace(c);
        if (*c->json == ',') {
            c->json++;
            lept_parse_whitespace(c);
        }
        else if(*c->json == ']')
        {
            c->json++;
            lept_set_array(v, size);
            memcpy(v->u.a.e, lept_context_pop(c, size * sizeof(lept_value)), size * sizeof(lept_value));
            v->u.a.size = size;
            return LEPT_PARSE_OK;
        }
        else{
            ret = LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
            break;
        }
    }
     /* Pop and free values on the stack */
    for (int i = 0; i < size; i++)
        lept_free((lept_value*)lept_context_pop(c, sizeof(lept_value)));
    return ret;
}

static int lept_parse_object(lept_context* c, lept_value* v) {
    size_t i, size;
    lept_member m;
    int ret;
    EXPECT(c, '{');
    lept_parse_whitespace(c);
    if (*c->json == '}') {
        c->json++;
        lept_set_object(v, 0);
        return LEPT_PARSE_OK;
    }
    m.k = NULL;
    size = 0;
    for (;;) {
        char* str;
        lept_init(&m.v);
        /* parse key */
        if (*c->json != '"') {
            ret = LEPT_PARSE_MISS_KEY;
            break;
        }
        if ((ret = lept_parse_string_raw(c, &str, &m.klen)) != LEPT_PARSE_OK)
            break;
        memcpy(m.k = (char*)malloc(m.klen + 1), str, m.klen);
        m.k[m.klen] = '\0';
        /* parse ws colon ws */
        lept_parse_whitespace(c);
        if (*c->json != ':') {
            ret = LEPT_PARSE_MISS_COLON;
            break;
        }
        c->json++;
        lept_parse_whitespace(c);
        /* parse value */
        if ((ret = lept_parse_value(c, &m.v)) != LEPT_PARSE_OK)
            break;
        memcpy(lept_context_push(c, sizeof(lept_member)), &m, sizeof(lept_member));
        size++;
        m.k = NULL; /* ownership is transferred to member on stack */
        /* parse ws [comma | right-curly-brace] ws */
        lept_parse_whitespace(c);
        if (*c->json == ',') {
            c->json++;
            lept_parse_whitespace(c);
        }
        else if (*c->json == '}') {
            size_t s = sizeof(lept_member) * size;
            c->json++;
            lept_set_object(v, size);
            memcpy(v->u.o.m, lept_context_pop(c, sizeof(lept_member) * size), sizeof(lept_member) * size);
            v->u.o.size = size;
            return LEPT_PARSE_OK;
        }
        else {
            ret = LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET;
            break;
        }
    }
    /* Pop and free members on the stack */
    free(m.k);
    for (i = 0; i < size; i++) {
        lept_member* m = (lept_member*)lept_context_pop(c, sizeof(lept_member));
        free(m->k);
        lept_free(&m->v);
    }
    v->type = LEPT_NULL;
    return ret;
}

static int lept_parse_value(lept_context* c,lept_value* v)
{
    /**
     * switch语句中default之前的case按顺序执行（首先确定是否命中case，命中case，从命中case开始执行），
     * 最后则执行default，如果在default 之后没有break，则继续执行default之后的语句，直到跳出switch。
    */
    switch(*c->json)
    {
        case 'n':   return lept_parse_literal(c,v,"null",LEPT_NULL);
        case 't':   return lept_parse_literal(c,v,"true",LEPT_TRUE);
        case 'f':   return lept_parse_literal(c,v,"false",LEPT_FALSE);
        default:    return lept_parse_number(c,v);
        case '"':   return lept_parse_string(c,v);
        case '[':  return lept_parse_array(c, v);
        case '{':  return lept_parse_object(c, v);
        case '\0':  return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v,const char* json)
{
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    c.stack = NULL;
    c.size = c.top = 0;
    // v->type = LEPT_NULL;
    lept_init(v);
    lept_parse_whitespace(&c);
    if((ret = lept_parse_value(&c,v)) == LEPT_PARSE_OK)
    {
        lept_parse_whitespace(&c);
        if(*c.json != '\0'){
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
            
    }
    assert(c.top == 0);
    free(c.stack);
    return ret;
}
#if 0
// Unoptimized
static void lept_stringify_string(lept_context* c, const char* s, size_t len) {
    size_t i;
    assert(s != NULL);
    PUTC(c, '"');
    for (i = 0; i < len; i++) {
        unsigned char ch = (unsigned char)s[i];
        switch (ch) {
            case '\"': PUTS(c, "\\\"", 2); break;
            case '\\': PUTS(c, "\\\\", 2); break;
            case '\b': PUTS(c, "\\b",  2); break;
            case '\f': PUTS(c, "\\f",  2); break;
            case '\n': PUTS(c, "\\n",  2); break;
            case '\r': PUTS(c, "\\r",  2); break;
            case '\t': PUTS(c, "\\t",  2); break;
            default:
                if (ch < 0x20) {
                    char buffer[7];
                    sprintf(buffer, "\\u%04X", ch);
                    PUTS(c, buffer, 6);
                }
                else
                    PUTC(c, s[i]);
        }
    }
    PUTC(c, '"');
}
#else
static void lept_stringify_string(lept_context* c,const char* s,size_t len)
{
    static const char hex_digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    size_t i,size;
    char* head,*p;
    assert(s != NULL);
    p = head = lept_context_push(c,size = len * 6 + 2);/* "\u00xx..." */
    *p++ = '"';
     for (i = 0; i < len; i++) {
        unsigned char ch = (unsigned char)s[i];
        switch (ch) {
            case '\"': *p++ = '\\'; *p++ = '\"'; break;
            case '\\': *p++ = '\\'; *p++ = '\\'; break;
            case '\b': *p++ = '\\'; *p++ = 'b';  break;
            case '\f': *p++ = '\\'; *p++ = 'f';  break;
            case '\n': *p++ = '\\'; *p++ = 'n';  break;
            case '\r': *p++ = '\\'; *p++ = 'r';  break;
            case '\t': *p++ = '\\'; *p++ = 't';  break;
            default:
                //%04x
                /**
                 * x表示以小写的十六进制数输出
                 * 4表示输出的十六进制数的宽度是4个字符；
                 * 0表示输出的十六进制数中，不足4个字符的部分，用“0”来补度充
                 * 
                 * 0x20表示ascii空格之前的字符 前32位控制字符
                */
                if (ch < 0x20) {//0x12  见demo.c测试  输出\u0012
                    *p++ = '\\'; *p++ = 'u'; *p++ = '0'; *p++ = '0';
                    *p++ = hex_digits[ch >> 4];//1
                    *p++ = hex_digits[ch & 15];//2
                }
                else
                    *p++ = s[i];
        }
    }
    *p++ = '"';
    c->top -= size - (p - head);
}
#endif
static void lept_stringify_value(lept_context* c,const lept_value* v)
{
    size_t i;
    switch (v->type)
    {
    case LEPT_NULL:
        PUTS(c,"null",4);break;
        break;
    case LEPT_TRUE:
        PUTS(c,"true",4);break;
        break;
    case LEPT_FALSE:
        PUTS(c,"false",5);break;
        break;
    case LEPT_NUMBER:
        c->top -= 32 - sprintf(lept_context_push(c,32),"%.17g",v->u.n);
        break;
    case LEPT_STRING:
        lept_stringify_string(c,v->u.s.s,v->u.s.len);
        break;
    
    case LEPT_ARRAY:
        PUTC(c,'[');
        for(i = 0; i < v->u.a.size;i++)
        {
            if(i > 0)
                PUTC(c,',');
            lept_stringify_value(c,&v->u.a.e[i]);
        }
        PUTC(c,']');
        break;  
    case LEPT_OBJECT:
        PUTC(c,'{');
        for(i = 0; i < v->u.o.size;i++){
            if(i > 0) PUTC(c,',');
            lept_stringify_string(c,v->u.o.m[i].k, v->u.o.m[i].klen);
            PUTC(c,':');
            lept_stringify_value(c,&v->u.o.m[i].v);
        }
        PUTC(c,'}');
        break;
    default:
        assert(0 && "invalid type");
        break;
    }
}

char* lept_stringify(const lept_value* v,size_t* length)
{
    lept_context c;
    assert(v != NULL);
    c.stack = (char*)malloc(c.size = LEPT_PARSE_STRINGIFY_INIT_SIZE);
    c.top = 0;
    lept_stringify_value(&c,v);
    if(length)
        *length = c.top;
    PUTC(&c,'\0');
    return c.stack;
}

void lept_copy(lept_value* dst, const lept_value* src) {
    assert(src != NULL && dst != NULL && src != dst);
    size_t i;
    switch (src->type) {
        case LEPT_STRING:
            lept_set_string(dst, src->u.s.s, src->u.s.len);
            break;
        case LEPT_ARRAY:
            /* \todo */
            //数组 先设置大小
            lept_set_array(dst,src->u.a.size);
            //逐个拷贝
            for(i = 0; i < src->u.a.size;i++)
            {
                lept_copy(&dst->u.a.e[i],&src->u.a.e[i]);
            }
            dst->u.a.size = src->u.a.size;
            break;
        case LEPT_OBJECT:
            /* \todo */
            //对象
            //先设置大小
            lept_set_object(dst,src->u.o.size);
            for(i = 0; i < src->u.o.size; i++)
            {
                //key
                // 设置k字段为key的对象的值，如果在查找过程中找到了已经存在key，则返回；否则新申请一块空间并初始化，然后返回
                lept_value* val = lept_set_object_value(dst,src->u.o.m[i].k,src->u.o.m[i].klen);
                //value
                lept_copy(val,&src->u.o.m[i].v);
            }
            dst->u.o.size = src->u.o.size;
            break;
        default:
            lept_free(dst);
            memcpy(dst, src, sizeof(lept_value));
            break;
    }
}

void lept_move(lept_value* dst, lept_value* src) {
    assert(dst != NULL && src != NULL && src != dst);
    lept_free(dst);
    memcpy(dst, src, sizeof(lept_value));
    lept_init(src);
}

void lept_swap(lept_value* lhs, lept_value* rhs) {
    assert(lhs != NULL && rhs != NULL);
    if (lhs != rhs) {
        lept_value temp;
        memcpy(&temp, lhs, sizeof(lept_value));
        memcpy(lhs,   rhs, sizeof(lept_value));
        memcpy(rhs, &temp, sizeof(lept_value));
    }
}

int lept_is_equal(const lept_value* lhs, const lept_value* rhs) {
    size_t i;
    assert(lhs != NULL && rhs != NULL);
    if (lhs->type != rhs->type)
        return 0;
    switch (lhs->type) {
        case LEPT_STRING:
            return lhs->u.s.len == rhs->u.s.len && 
                memcmp(lhs->u.s.s, rhs->u.s.s, lhs->u.s.len) == 0;
        case LEPT_NUMBER:
            return lhs->u.n == rhs->u.n;
        case LEPT_ARRAY:
            if (lhs->u.a.size != rhs->u.a.size)
                return 0;
            for (i = 0; i < lhs->u.a.size; i++)
                if (!lept_is_equal(&lhs->u.a.e[i], &rhs->u.a.e[i]))
                    return 0;
            return 1;
        case LEPT_OBJECT:
            /* \todo */
            //对于对象 先比较键值个数是否一样
            //之后对于左边的键值对，依次在右边进行查找
            if(lhs->u.o.size != rhs->u.o.size){
                return 0;
            }
            for(i = 0; i < lhs->u.o.size;i++)
            {
                size_t index = lept_find_object_index(rhs,lhs->u.o.m[i].k,lhs->u.o.m[i].klen);
                /*key not exist*/
                if(index == LEPT_KEY_NOT_EXIST){
                    return 0;
                }
                /*key exists,but value not match*/
                if(!lept_is_equal(&lhs->u.o.m[i].v,&rhs->u.o.m[index].v))
                {
                    return 0;
                }
            }
            return 1;
        default://对于null true false只要类型值相同 即证明值相同
            return 1;
    }
}

//释放lept_value 动态分配的内存
void lept_free(lept_value* v)
{
    size_t i;
    assert(v != NULL);
    switch(v->type)
    {
        case LEPT_STRING:
            free(v->u.s.s);
            break;
        case LEPT_ARRAY:
            for(i = 0;i < v->u.a.size;i++)
                lept_free(&v->u.a.e[i]);
            free(v->u.a.e);
            break;
        case LEPT_OBJECT:
            for(i = 0; i < v->u.o.size;i++)
            {
                free(v->u.o.m[i].k);
                lept_free(&v->u.o.m[i].v);
            }
            free(v->u.o.m);
            break;
        default: break;
    }
    v->type = LEPT_NULL;
}

lept_type lept_get_type(const lept_value* v)
{
    assert(v != NULL);
    return v->type;
}

int lept_get_boolean(const lept_value* v) {
    assert(v != NULL && (v->type == LEPT_FALSE || v->type == LEPT_TRUE));
    return v->type == LEPT_TRUE;
}

void lept_set_boolean(lept_value* v, int b) {
   lept_free(v);
   v->type = b ? LEPT_TRUE : LEPT_FALSE;
}

double lept_get_number(const lept_value* v)
{
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->u.n;

}

void lept_set_number(lept_value* v, double n) {
    lept_free(v);
    v->u.n = n;
    v->type = LEPT_NUMBER;
}

const char* lept_get_string(const lept_value* v)
{
    assert(v != NULL && v->type == LEPT_STRING);
    return v->u.s.s;
}

size_t lept_get_string_length(const lept_value* v)
{
    assert(v != NULL && v->type == LEPT_STRING);
    return v->u.s.len;
}

void lept_set_string(lept_value* v,const char* s,size_t len)
{
    assert(v != NULL && (s != NULL || len == 0));
    lept_free(v);
    v->u.s.s = (char*)malloc(len + 1);
    memcpy(v->u.s.s,s,len);
    v->u.s.s[len] = '\0';
    v->u.s.len = len;
    v->type = LEPT_STRING;

}

void lept_set_array(lept_value* v, size_t capacity)
{
    assert(v != NULL);
    lept_free(v);
    v->type = LEPT_ARRAY;
    v->u.a.size = 0;
    v->u.a.capacity = capacity;
    v->u.a.e = capacity > 0 ? (lept_value*)malloc(capacity * sizeof(lept_value)) : NULL;
}


size_t lept_get_array_size(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_ARRAY);
    return v->u.a.size;
}

size_t lept_get_array_capacity(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_ARRAY);
    return v->u.a.capacity;
}

void lept_reserve_array(lept_value* v,size_t capacity)
{
    assert(v != NULL && v->type == LEPT_ARRAY);
    if(v->u.a.capacity < capacity)
    {
        v->u.a.capacity = capacity;
        v->u.a.e = (lept_value*)realloc(v->u.a.e,v->u.a.capacity * sizeof(lept_value));
    }
}

void lept_shrink_array(lept_value* v) {
    assert(v != NULL && v->type == LEPT_ARRAY);
    if (v->u.a.capacity > v->u.a.size) {
        v->u.a.capacity = v->u.a.size;
        v->u.a.e = (lept_value*)realloc(v->u.a.e, v->u.a.capacity * sizeof(lept_value));
    }
}

void lept_clear_array(lept_value* v) {
    assert(v != NULL && v->type == LEPT_ARRAY);
    lept_erase_array_element(v, 0, v->u.a.size);
}

lept_value* lept_get_array_element(lept_value* v, size_t index) {
    assert(v != NULL && v->type == LEPT_ARRAY);
    assert(index < v->u.a.size);
    return &v->u.a.e[index];
}

lept_value* lept_pushback_array_element(lept_value* v) {
    assert(v != NULL && v->type == LEPT_ARRAY);
    if (v->u.a.size == v->u.a.capacity)
        lept_reserve_array(v, v->u.a.capacity == 0 ? 1 : v->u.a.capacity * 2);
    lept_init(&v->u.a.e[v->u.a.size]);
    return &v->u.a.e[v->u.a.size++];
}

void lept_popback_array_element(lept_value* v) {
    assert(v != NULL && v->type == LEPT_ARRAY && v->u.a.size > 0);
    lept_free(&v->u.a.e[--v->u.a.size]);
}

lept_value* lept_insert_array_element(lept_value* v, size_t index) {
    assert(v != NULL && v->type == LEPT_ARRAY && index <= v->u.a.size);
    /* \todo */
    // index不可以超过size（因为是插入）（等于的话，相当于插在末尾）
    lept_value* val = lept_pushback_array_element(v);
    if(index == (v->u.a.size - 1)) return val;
    memcpy(&v->u.a.e[index + 1],&v->u.a.e[index],sizeof(lept_value) * (v->u.a.size - index - 1));
    lept_init(&v->u.a.e[index]);
    return &v->u.a.e[index];
}

void lept_erase_array_element(lept_value* v, size_t index, size_t count) {
    assert(v != NULL && v->type == LEPT_ARRAY && index + count <= v->u.a.size);
    /* \todo */
    // 回收完空间，然后将index 后面count个元素移到index处，最后将空闲的count个元素重新初始化
    size_t i;
    for(i = index; i < index + count;++i)
    {
        lept_free(&v->u.a.e[i]);
    }
    memcpy(&v->u.a.e[index],&v->u.a.e[index + count],sizeof(lept_value) * (v->u.a.size - index - count));
    for(i = v->u.a.size - count; i < v->u.a.size;++i)
    {
        lept_init(&v->u.a.e[i]);
    }
    v->u.a.size -= count;
}

void lept_set_object(lept_value* v,size_t capacity)
{
    assert(v != NULL);
    lept_free(v);
    v->type = LEPT_OBJECT;
    v->u.o.size = 0;
    v->u.o.capacity = capacity;
    v->u.o.m = capacity > 0 ? (lept_member*)malloc(capacity * sizeof(lept_member)) : NULL;
}


size_t lept_get_object_size(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_OBJECT);
    return v->u.o.size;
}

size_t lept_get_object_capacity(const lept_value* v) 
{
    assert(v != NULL && v->type == LEPT_OBJECT);
    return v->u.o.capacity;
}

void lept_reserve_object(lept_value* v, size_t capacity) {
    assert(v != NULL && v->type == LEPT_OBJECT);
    /* \todo */
    /*重置对象容量 */
    if(v->u.o.size < capacity){
        v->u.o.capacity = capacity;
        v->u.o.m = (lept_member*)realloc(v->u.o.m, sizeof(lept_member) * v->u.o.capacity );
    }
}

void lept_shrink_object(lept_value* v) {
    assert(v != NULL && v->type == LEPT_OBJECT);
    /* \todo */
    /*收缩对象容器大小 到符合大小*/
    if(v->u.o.size < v->u.o.capacity)
    {
        v->u.o.capacity = v->u.o.size;
        v->u.o.m = (lept_member*)realloc(v->u.o.m,sizeof(lept_member) * v->u.o.capacity);
    }
}

void lept_clear_object(lept_value* v) {
    assert(v != NULL && v->type == LEPT_OBJECT);
    /* \todo */
    /*清空对象*/
    size_t i;
    for(i = 0; i < v->u.o.size; ++i)
    {
        //回收 key 和 value 的空间
        free(v->u.o.m[i].k);
        v->u.o.m[i].k = NULL;
        v->u.o.m[i].klen = 0;
        lept_free(&v->u.o.m[i].v);
    }
    v->u.o.size = 0;
}

const char* lept_get_object_key(const lept_value* v, size_t index) {
    assert(v != NULL && v->type == LEPT_OBJECT);
    assert(index < v->u.o.size);
    return v->u.o.m[index].k;
}

size_t lept_get_object_key_length(const lept_value* v, size_t index) {
    assert(v != NULL && v->type == LEPT_OBJECT);
    assert(index < v->u.o.size);
    return v->u.o.m[index].klen;
}

lept_value* lept_get_object_value(lept_value* v, size_t index) {
    assert(v != NULL && v->type == LEPT_OBJECT);
    assert(index < v->u.o.size);
    return &v->u.o.m[index].v;
}

size_t lept_find_object_index(const lept_value* v, const char* key, size_t klen)
{
    size_t i;
    assert(v != NULL && v->type == LEPT_OBJECT && key != NULL);
    for(i = 0; i < v->u.o.size;i++)
    {
        if(v->u.o.m[i].klen == klen && memcmp(v->u.o.m[i].k ,key,klen) == 0)
            return i;
    }
    return LEPT_KEY_NOT_EXIST;
}

lept_value* lept_find_object_value(lept_value* v, const char* key, size_t klen)
{
    size_t index = lept_find_object_index(v,key,klen);
    return index != LEPT_KEY_NOT_EXIST ? &v->u.o.m[index].v : NULL;
}

lept_value* lept_set_object_value(lept_value* v, const char* key, size_t klen) {
    assert(v != NULL && v->type == LEPT_OBJECT && key != NULL);
    /* \todo */
    // 设置k字段为key的对象的值，如果在查找过程中找到了已经存在key，则返回；否则新申请一块空间并初始化，然后返回
    size_t i,index;
    index = lept_find_object_index(v,key,klen);
    if(index != LEPT_KEY_NOT_EXIST)
    {
        return &v->u.o.m[index].v;
    }

    //key not exists, then we make space to return
    if(v->u.o.size == v->u.o.capacity)
    {
        lept_reserve_object(v,v->u.o.capacity == 0 ? 1 : (v->u.o.capacity << 1));
    }
    i = v->u.o.size;
    v->u.o.m[i].k = (char*)malloc(klen + 1);
    memcpy(v->u.o.m[i].k, key, klen);
    v->u.o.m[i].k[klen] = '\0';
    v->u.o.m[i].klen = klen;
    lept_init(&v->u.o.m[i].v);
    v->u.o.size++;
    return &v->u.o.m[i].v;
}

void lept_remove_object_value(lept_value* v, size_t index) {
    assert(v != NULL && v->type == LEPT_OBJECT && index < v->u.o.size);
    /* \todo */
    free(v->u.o.m[index].k);
    lept_free(&v->u.o.m[index].v);
    memcpy(&v->u.o.m[index],&v->u.o.m[index + 1],sizeof(lept_member)*(v->u.o.size - index - 1));

    v->u.o.size--;
    v->u.o.m[v->u.o.size].k = NULL;
    v->u.o.m[v->u.o.size].klen = 0;
    lept_init(&v->u.o.m[v->u.o.size].v);
}
