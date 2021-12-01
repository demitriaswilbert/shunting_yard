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

int main()
{
    Token_t tokens[100];
    int tFilled = 0;
    int type = 0;
    int tmpval = 0;
    double dval = 0;
    int comma = 0;
    for(int ch = fgetc(stdin); ch >= 32 && ch <= 127; ch = fgetc(stdin))
    {
        if(ch == ' ')
        {
            if(type == 1)
            {
                append_token(tokens, type, *((uint64_t*)&dval), &tFilled);
                type = 0;
                dval = 0;
                comma = 0;
                tmpval = 0;
            }
            if(type != 0)
            {
                append_token(tokens, type, tmpval, &tFilled);
                type = 0;
                tmpval = 0;
            }
        }
        if((ch >= '0' && ch <= '9') || ch == '.')
        {
            if(type == 2)
            {
                append_token(tokens, type, tmpval, &tFilled);
                type = 0;
                tmpval = 0;
                comma = 0;
                dval = 0;
            }
            if(type == 0)
            {
                tmpval = 0;
                type = 1;
                comma = 0;
                dval = 0;
            }
            if(type == 1)
            {
                if(ch == '.')comma++;
                else if(comma == 0)
                {
                    dval *= 10;
                    dval += (int)(ch - '0');
                }
                else
                {
                    double digit = (int)(ch - '0');
                    for(int c = 0; c < comma; c++)
                        digit /= 10;
                    dval += digit;
                    comma++;
                }
            }
        }
        if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')')
        {
            if(type == 1)
            {
                append_token(tokens, type, *((uint64_t*)&dval), &tFilled);
                dval = 0;
                comma = 0;
                type = 0;
                tmpval = 0;
            }
            type = 2;
            tmpval = ch;
            append_token(tokens, type, tmpval, &tFilled);
            type = 0;
            tmpval = 0;
        }
    }
    if(type == 1)
    {
        append_token(tokens, type, *((uint64_t*)&dval), &tFilled);
        type = 0;
        tmpval = 0;
        dval = 0;
        comma = 0;
    }

    append_token(tokens, 3, 0, &tFilled);
    

    for(int i = 0; i < tFilled; i++)
    {
        if(tokens[i].type == 0)
            printf(" \n");
        if(tokens[i].type == 1)
            printf("%lf\n", *((double*)&tokens[i].val));
        if(tokens[i].type == 2)
            printf("%c\n", (char)tokens[i].val);
        if(tokens[i].type == 3)
            printf("End\n");
    }
}