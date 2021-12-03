/**
 * @file a.c
 * @author Demitrias Wilbert (demitriaswilbert@gmail.com)
 * @brief Source code for Shunting yard algorithm
 * @version 0.1
 * @date 2021-12-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int type;
    union
    {
        uint64_t u64;
        double f64;
    } value;
} Token_t;

int prec(Token_t op)
{
    char c = op.value.u64;
    if (c == '/' || c == '*')
        return 2;
    if (c == '+' || c == '-')
        return 1;
    return -1;
}

void push_token(Token_t* buf, int type, uint64_t val, int* nfilled)
{
    Token_t tmpval = {type, {val}};
    buf[(*nfilled)++] = tmpval;
}

void pop_token(Token_t* dest, int* destFilled, Token_t* src, int* srcFilled)
{
    dest[(*destFilled)++] = src[--(*srcFilled)];
}

void print_queue(Token_t* queue, int nqueue)
{
    for (int i = 0; i < nqueue; i++)
    {
        if (queue[i].type == 1)
            printf("%lg ", queue[i].value.f64);
        else if (queue[i].type == 2)
            printf("%c ", (char)queue[i].value.u64);
    }
    printf("\n");
}

Token_t operate(Token_t* t1, Token_t* t2, Token_t* operator)
{
    Token_t result = {1, {0ULL}};
    switch ((char)operator->value.u64)
    {
        case '+':
        {
            result.value.f64 = t1->value.f64 + t2->value.f64;
            break;
        }
        case '-':
        {
            result.value.f64 = t1->value.f64 - t2->value.f64;
            break;
        }
        case '*':
        {
            result.value.f64 = t1->value.f64 * t2->value.f64;
            break;
        }
        case '/':
        {
            result.value.f64 = t1->value.f64 / t2->value.f64;
            break;
        }
    }
    printf("operate : %lg %c %lg = %lg\n", t1->value.f64, (char)operator->value.u64, t2->value.f64, result.value.f64);
    return result;
}

int process_token(Token_t* tokens, int* ptFilled)
{
    int tFilled = *ptFilled;
    if (tFilled < 3) return -1;
    Token_t space = {0, {0ULL}};
    Token_t *tOp = NULL, *t1 = NULL, *t0 = NULL;
    int i = 0;
    for (; i < tFilled; i++)
    {
        if (tokens[i].type == 2)
        {
            tOp = &tokens[i];
            i--;
            break;
        }
    }
    if (tOp == NULL) return 0;

    for (; i >= 1; i--)
    {
        if (tokens[i].type == 1)
        {
            t1 = &tokens[i];
            i--;
            break;
        }
    }
    if (t1 == NULL) return -1;

    for (; i >= 0; i--)
    {
        if (tokens[i].type == 1)
        {
            t0 = &tokens[i];
            i--;
            break;
        }
    }
    if (t0 == NULL) return -1;

    *tOp = operate(t0, t1, tOp);
    *t0 = space; *t1 = space;
    return 1;
}

void syAlg(Token_t* opstack, int* nopstack, Token_t* queue, int* nqueue, Token_t token)
{
    if (token.type == 1)
    {
        push_token(queue, token.type, token.value.u64, nqueue);
    }
    if (token.type == 2)
    {
        if (token.value.u64 == '(')
            push_token(opstack, token.type, token.value.u64, nopstack);
        else if (token.value.u64 == ')')
        {
            while (opstack[(*nopstack) - 1].value.u64 != '(')
            {
                pop_token(queue, nqueue, opstack, nopstack);
                if (*nopstack == 0)
                {
                    fprintf(stderr, "ERROR. mismatched parentheses\n");
                    exit(-1);
                }
            }
            (*nopstack)--;
        }
        else
        {
            while (((*nopstack) > 0) && prec(token) <= prec(opstack[(*nopstack) - 1]))
            {
                pop_token(queue, nqueue, opstack, nopstack);
            }
            push_token(opstack, token.type, token.value.u64, nopstack);
        }
    }
}

int main()
{
    Token_t opstack[1000]; int nopstack = 0;
    Token_t queue[1000];   int nqueue = 0;

    Token_t token;
    int comma = 0;
    for (int ch = fgetc(stdin); ch >= 32 && ch <= 127; ch = fgetc(stdin))
    {
        if (ch == ' ')
        {
            if (token.type == 1)
                syAlg(opstack, &nopstack, queue, &nqueue, token);
            token = (Token_t) {0, {0ULL}};
        }
        if ((ch >= '0' && ch <= '9') || ch == '.' || ch == ',')
        {
            if (token.type != 1)
            { comma = 0; token = (Token_t) {1, {0.0}}; }
            if ((ch == '.' || ch == ',') && comma == 0)
                comma++;
            else if (comma == 0)
            {
                token.value.f64 *= 10;
                token.value.f64 += (int)(ch - '0');
            }
            else
            {
                double digit = (int)(ch - '0');
                for (int c = 0; c < comma; c++)
                    digit /= 10;
                token.value.f64 += digit; comma++;
            }
        }
        if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')')
        {
            if (token.type == 1)
                syAlg(opstack, &nopstack, queue, &nqueue, token);
            token = (Token_t) {2, {(uint64_t)ch}};
            syAlg(opstack, &nopstack, queue, &nqueue, token);
        }
    }
    syAlg(opstack, &nopstack, queue, &nqueue, token);

    if ((opstack[nopstack - 1].type == 2) && (opstack[nopstack - 1].value.u64 == '('))
    {
        fprintf(stderr, "ERROR. mismatched parentheses\n");
        exit(-1);
    }
    while (nopstack)
    {
        pop_token(queue, &nqueue, opstack, &nopstack);
    }

    print_queue(queue, nqueue);
    int err = 1;
    while (err > 0)
    {
        err = process_token(queue, &nqueue);
        print_queue(queue, nqueue);
    }
    if (err == -1) printf("error -1\n");
}
