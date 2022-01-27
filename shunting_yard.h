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
#include <math.h>

typedef struct
{
    int type;
    union
    {
        uint64_t u64;
        double f64;
    } value;
} Token_t;

Token_t Calculate(char* expr);
