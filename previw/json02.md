解析数字

## 1.初探重构
在讨论解析数字之前，我们再补充 TDD 中的一个步骤──重构（refactoring）。重构是一个这样的过程：

> 在不改变代码外在行为的情况下，对代码作出修改，以改进程序的内部结构。

在 TDD 的过程中，我们的目标是编写代码去通过测试。但由于这个目标的引导性太强，我们可能会忽略正确性以外的软件品质。在通过测试之后，代码的正确性得以保证，我们就应该审视现时的代码，看看有没有地方可以改进，而同时能维持测试顺利通过。我们可以安心地做各种修改，因为我们有单元测试，可以判断代码在修改后是否影响原来的行为。

那么，哪里要作出修改？Beck 和 Fowler认为程序员要培养一种判断能力，找出程序中的坏味道。例如，在第一单元的练习中，可能大部分人都会复制 lept_parse_null() 的代码，作一些修改，成为 lept_parse_true() 和 lept_parse_false()。如果我们再审视这 3 个函数，它们非常相似。这违反编程中常说的 DRY（don't repeat yourself）原则。本单元的第一个练习题，就是尝试合并这 3 个函数。

另外，我们也可能发现，单元测试代码也有很重复的代码，例如 test_parse_invalid_value() 中我们每次测试一个不合法的 JSON 值，都有 4 行相似的代码。我们可以把它用宏的方式把它们简化：
```C
#define TEST_ERROR(error, json)\
    do {\
        lept_value v;\
        v.type = LEPT_FALSE;\
        EXPECT_EQ_INT(error, lept_parse(&v, json));\
        EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));\
    } while(0)

static void test_parse_expect_value() {
    TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, "");
    TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, " ");
}
```
## JSON数字语法
回归正题，本单元的重点在于解析 JSON number 类型。我们先看看它的语法：
```C
number = [ "-" ] int [ frac ] [ exp ]
int = "0" / digit1-9 *digit
frac = "." 1*digit
exp = ("e" / "E") ["-" / "+"] 1*digit
```
number 是以十进制表示，它主要由 4 部分顺序组成：负号、整数、小数、指数。只有整数是必需部分。注意和直觉可能不同的是，正号是不合法的。

整数部分如果是 0 开始，只能是单个 0；而由 1-9 开始的话，可以加任意数量的数字（0-9）。也就是说，0123 不是一个合法的 JSON 数字。

小数部分比较直观，就是小数点后是一或多个数字（0-9）。

JSON 可使用科学记数法，指数部分由大写 E 或小写 e 开始，然后可有正负号，之后是一或多个数字（0-9）。

JSON 标准 ECMA-404 采用图的形式表示语法，也可以更直观地看到解析时可能经过的路径：
![](./number.png)
上一单元的 null、false、true 在解析后，我们只需把它们存储为类型。但对于数字，我们要考虑怎么存储解析后的结果。

## 3.数字表示方式
从 JSON 数字的语法，我们可能直观地会认为它应该表示为一个浮点数（floating point number），因为它带有小数和指数部分。然而，标准中并没有限制数字的范围或精度。为简单起见，leptjson 选择以双精度浮点数（C 中的 double 类型）来存储 JSON 数字。
我们为 lept_value 添加成员：
```C
typedef struct {
    double n;
    lept_type type;
}lept_value;
```
仅当 type == LEPT_NUMBER 时，n 才表示 JSON 数字的数值。所以获取该值的 API 是这么实现的：
```C
double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
```
使用者应确保类型正确，才调用此 API。我们继续使用断言来保证。
## 4.单元测试
我们定义了 API 之后，按照 TDD，我们可以先写一些单元测试。这次我们使用多行的宏的减少重复代码：

```C
#define TEST_NUMBER(expect, json)\
    do {\
        lept_value v;\
        EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, json));\
        EXPECT_EQ_INT(LEPT_NUMBER, lept_get_type(&v));\
        EXPECT_EQ_DOUBLE(expect, lept_get_number(&v));\
    } while(0)

static void test_parse_number() {
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
}
```
以上这些都是很基本的测试用例，也可供调试用。大部分情况下，测试案例不能穷举所有可能性。因此，除了加入一些典型的用例，我们也常会使用一些边界值，例如最大值等。练习中会让同学找一些边界值作为用例。

除了这些合法的 JSON，我们也要写一些不合语法的用例：
```C
static void test_parse_invalid_value() {
    /* ... */
    /* invalid number */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nan");
}
```
## 5.十进制转换为二进制
我们需要把十进制的数字转换成二进制的 double。这并不是容易的事情 。为了简单起见，leptjson 将使用标准库的 strtod() 来进行转换。strtod() 可转换 JSON 所要求的格式，但问题是，一些 JSON 不容许的格式，strtod() 也可转换，所以我们需要自行做格式校验。
```C
#include <stdlib.h>  /* NULL, strtod() */

static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;
    /* \TODO validate number */
    v->n = strtod(c->json, &end);
    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}
```
加入了 number 后，value 的语法变成：
```
value = null / false / true / number
```
记得在第一单元中，我们说可以用一个字符就能得知 value 是什么类型，有 11 个字符可判断 number：
- 0-9/- ➔ number 0-9/- ➔ 数量

但是，由于我们在 lept_parse_number() 内部将会校验输入是否正确的值，我们可以简单地把余下的情况都交给 lept_parse_number()：
```C
static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't':  return lept_parse_true(c, v);
        case 'f':  return lept_parse_false(c, v);
        case 'n':  return lept_parse_null(c, v);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}
```
## 问题
- 为什么要把一些测试代码以 #if 0 ... #endif 禁用？

使用 #if 0 ... #endif 而不使用 /* ... */，是因为 C 的注释不支持嵌套（nested），而 #if ... #endif 是支持嵌套的。代码中已有注释时，用 #if 0 ... #endif 去禁用代码是一个常用技巧，而且可以把 0 改为 1 去恢复。

- 科学计数法的指数部分没有对前导零作限制吗？1E012 也是合法的吗？

是的，这是合法的。JSON 源自于 JavaScript（ECMA-262, 3rd edition），数字语法取自 JavaScript 的十进位数字的语法（§7.8.3 Numeric Literals）。整数不容许前导零（leading zero），是因为更久的 JavaScript 版本容许以前导零来表示八进位数字，如 052 == 42，这种八进位常数表示方式来自于 C 语言。禁止前导零避免了可能出现的歧义。但是在指数里就不会出现这个问题。
## 总结
在TDD的过程中，我们过分注重目标而很容易忽略过程，所以在实现功能之后，需要对代码进行一定的重构(在不影响代码外部行为的情况下，对代码进行修改，以改进程序的内部结构)。在本部分，宏函数的定义是一个很好的重构这个概念的实施，还有对于 NULL ，FALSE ，TRUE这三种字面量的解析过程也是基本类似的，可以进行合并和重构。

在使用strtod之前需要做一些类型校验，检验一些非法的strtod不能检测的类型错误。在通过类型判断之后在利用strtod函数将字符串转换为double类型的数字。

- 数字类型格式 [-] num [.][e]
首先第一个字符是 负号就跳过。如果是正号表示不合法。对于小数点前的数字，要么是单个0，要么是首位一个不为0的数字加0到多个0~9的数字，如果有小数点出现，需要判断小数点的下一位是否为合法的0~9的数字。对于后面出现科学计数法的e|E，下一位可以有 + | -，直接跳过，在之后需要跟着1到多个合法的数字。如果满足这些要求，进行strtod识别，在识别完成之后需要判断是否超过了double的最大值和最小值的界限(超过界限，errno会置为ERANGE，并且返回HUGE_VAL)。如果这些没有问题，则解析数字成功。
