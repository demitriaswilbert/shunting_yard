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
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct
{
    int type;
    uint64_t val;
}Token_t;

int prec(Token_t op) {
    char c = op.val;
    if(c == '/' || c=='*')
        return 2;
    else if(c == '+' || c == '-')
        return 1;
    else
        return -1;
}

void push_token(Token_t* buf, int type, uint64_t val, int* nfilled)
{
    Token_t tmpval = {type, val};
    buf[(*nfilled)++] = tmpval;
}

void pop_token(Token_t* dest, int* destFilled, Token_t* src, int* srcFilled)
{
    dest[(*destFilled)++] = src[--(*srcFilled)];
}

void print_queue(Token_t* queue, int nqueue)
{
    for(int i = 0; i < nqueue; i++)
    {
        if(queue[i].type == 1)
            printf("%lg ", *((double*)&queue[i].val));
        else if(queue[i].type == 2)
            printf("%c ", (char)queue[i].val);
    }
    printf("\n");
}

Token_t operate(Token_t* t1, Token_t* t2, Token_t* operator)
{
    Token_t result = {1, 0ULL};
    switch ((char)operator->val) 
    {
        case '+': 
        {
            *((double*)&(result.val)) = *((double*)&(t1->val)) + *((double*)&(t2->val));
            break;
        }
        case '-': 
        {
            *((double*)&(result.val)) = *((double*)&(t1->val)) - *((double*)&(t2->val));
            break;
        }
        case '*': 
        {
            *((double*)&(result.val)) = *((double*)&(t1->val)) * *((double*)&(t2->val));
            break;
        }
        case '/': 
        {
            *((double*)&(result.val)) = *((double*)&(t1->val)) / *((double*)&(t2->val));
            break;
        }
    }
    printf("operate : %lg %c %lg = %lg\n", *(double*)&t1->val, (char)operator->val, *(double*)&t2->val, *(double*)&result.val);
    return result;
}

int process_token(Token_t* tokens, int* ptFilled)
{
    int tFilled = *ptFilled;
    if(tFilled < 3) return -1;
    Token_t space = {0, 0ULL};
    Token_t *tOp = NULL, *t1 = NULL, *t0 = NULL;
    int i = 0;
    for(; i < tFilled; i++)
    {
        if(tokens[i].type == 2)
        {tOp = &tokens[i]; i--; break;}
    }
    if(tOp == NULL)return 0;

    for(; i >= 1; i--)
    {
        if(tokens[i].type == 1)
        {t1 = &tokens[i]; i--; break;}
    }
    if(t1 == NULL)return -1;
    

    for(; i >= 0; i--)
    {
        if(tokens[i].type == 1)
        {t0 = &tokens[i]; i--; break;}
    }
    if(t0 == NULL)return -1;

    *tOp = operate(t0, t1, tOp);
    *t0 = space; *t1 = space;
    return 1;
}

void syAlg(Token_t* opstack, int* nopstack, Token_t* queue, int* nqueue, Token_t token)
{
    if(token.type == 1)
        push_token(queue, token.type, token.val, nqueue);
    if(token.type == 2)
    {
        if(token.val == '(')
            push_token(opstack, token.type, token.val, nopstack);
        else if(token.val == ')')
        {
            while(opstack[(*nopstack) - 1].val != '(')
            {
                pop_token(queue, nqueue, opstack, nopstack);
            }
            (*nopstack)--;
        }
        else
        {
            while(((*nopstack) > 0) && prec(token) <= prec(opstack[(*nopstack) - 1]))
            {
                pop_token(queue, nqueue, opstack, nopstack);
            }
            push_token(opstack, token.type, token.val, nopstack);
        }
    }
}

int main()
{
    Token_t opstack[1000];
    int nopstack = 0;
    Token_t queue[1000];
    int nqueue = 0;

    Token_t token;
    int comma = 0;
    for(int ch = fgetc(stdin); ch >= 32 && ch <= 127; ch = fgetc(stdin))
    {
        if(ch == ' ')
        {
            if(token.type == 1)
            {
                syAlg(opstack, &nopstack, queue, &nqueue, token);
                token.type = 0; comma = 0; token.val = 0;
            }
            if(token.type != 0)
            {
                syAlg(opstack, &nopstack, queue, &nqueue, token);
                token.type = 0; comma = 0; token.val = 0;
            }
        }
        if((ch >= '0' && ch <= '9') || ch == '.' || ch == ',')
        {
            if(token.type == 2)
            {
                syAlg(opstack, &nopstack, queue, &nqueue, token);
                token.type = 1; comma = 0; token.val = 0;
            }
            if(token.type == 0)
            { token.type = 1; comma = 0; token.val = 0; }
            if(token.type == 1)
            {
                if(ch == '.' || ch == ',')comma++;
                else if(comma == 0)
                {
                    *((double*)&token.val) *= 10;
                    *((double*)&token.val) += (int)(ch - '0');
                }
                else
                {
                    double digit = (int)(ch - '0');
                    for(int c = 0; c < comma; c++)
                        digit /= 10;
                    *((double*)&token.val) += digit;
                    comma++;
                }
            }
        }
        if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')')
        {
            if(token.type == 1)
            {
                token.val = token.val;
                syAlg(opstack, &nopstack, queue, &nqueue, token);
                token.type = 0; comma = 0; token.val = 0;
            }
            token.type = 2;
            token.val = ch;
            syAlg(opstack, &nopstack, queue, &nqueue, token);
            token.type = 0; comma = 0; token.val = 0;
        }
    }
    if(token.type == 1)
    {
        token.val = token.val;
        syAlg(opstack, &nopstack, queue, &nqueue, token);
        token.type = 0; comma = 0; token.val = 0;
    }

    while(nopstack)
    {
        pop_token(queue, &nqueue, opstack, &nopstack);
    }

    print_queue(queue, nqueue);
    int err = 1;
    while(err > 0)
    { 
        err = process_token(queue, &nqueue);
        print_queue(queue, nqueue);
    }
    if(err == -1) printf("error -1\n");
    
    //print_queue(queue, nqueue);
} 
