#pragma once

#define MAX_EXPR_LEN 100
#define MAX_NODES_COUNT 100
#define MAX_NODE_VALUE_LEN 10
#define MAX_STACK_SIZE 20

typedef enum
{
    VARIABLE,
    REAL_CONSTANT,
    IMAGINARY_CONSTANT,
    POSTFIX_FUNC, // like a factorial ( 5! )
    PREFIX_FUNC, // like a sin ( sin(x) )
    BIN_OPERATION, // 2+2, 2*2 etc.
    OPEN_BRACKET, // (
    CLOSING_BRACKET // )
} value_type;

typedef struct
{
    value_type type;
    char value[MAX_NODE_VALUE_LEN];
} node;


node* ec_convert_to_postfix(char infix_expr[]);