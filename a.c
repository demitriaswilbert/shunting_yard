#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct
{
    int type;
    uint64_t val;
}Token_t;

void append_token(Token_t* buf, int type, uint64_t val, int* nfilled)
{
    Token_t tmpval = {type, val};
    buf[(*nfilled)++] = tmpval;
}

void pop_token(Token_t* dest, int* destFilled, Token_t* src, int* srcFilled)
{
    dest[(*destFilled)++] = src[--(*srcFilled)];
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
    printf("%lf %c %lf = %lf\n", *(double*)&t1->val, (char)operator->val, *(double*)&t2->val, *(double*)&result.val);
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

int main()
{
    Token_t tokens[100];
    int tFilled = 0;
    int type = 0;
    uint64_t tmpval = 0;
    int comma = 0;
    for(int ch = fgetc(stdin); ch >= 32 && ch <= 127; ch = fgetc(stdin))
    {
        if(ch == ' ')
        {
            if(type == 1)
            {
                append_token(tokens, type, tmpval, &tFilled);
                type = 0; comma = 0; tmpval = 0;
            }
            if(type != 0)
            {
                append_token(tokens, type, tmpval, &tFilled);
                type = 0; comma = 0; tmpval = 0;
            }
        }
        if((ch >= '0' && ch <= '9') || ch == '.' || ch == ',')
        {
            if(type == 2)
            {
                append_token(tokens, type, tmpval, &tFilled);
                type = 1; comma = 0; tmpval = 0;
            }
            if(type == 0)
            { type = 1; comma = 0; tmpval = 0; }
            if(type == 1)
            {
                if(ch == '.' || ch == ',')comma++;
                else if(comma == 0)
                {
                    *((double*)&tmpval) *= 10;
                    *((double*)&tmpval) += (int)(ch - '0');
                }
                else
                {
                    double digit = (int)(ch - '0');
                    for(int c = 0; c < comma; c++)
                        digit /= 10;
                    *((double*)&tmpval) += digit;
                    comma++;
                }
            }
        }
        if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')')
        {
            if(type == 1)
            {
                append_token(tokens, type, tmpval, &tFilled);
                type = 0; comma = 0; tmpval = 0;
            }
            append_token(tokens, 2, ch, &tFilled);
            type = 0; comma = 0; tmpval = 0;
        }
    }
    if(type == 1)
    {
        append_token(tokens, type, tmpval, &tFilled);
        type = 0; comma = 0; tmpval = 0;
    }

    int err = 1;
    while(err > 0)
    { 
        err = process_token(tokens, &tFilled);
    }
    if(err == -1) printf("error -1\n");
    
    for(int i = 0; i < tFilled; i++)
    {
        if(tokens[i].type == 1)
            printf("%lf\n", *((double*)&tokens[i].val));
        else if(tokens[i].type == 2)
            printf("%c\n", (char)tokens[i].val);
        else if(tokens[i].type == 3)
            printf("End\n");
    }
} 
