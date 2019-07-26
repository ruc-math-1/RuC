//
//  scaner.c
//  RuC
//
//  Created by Andrey Terekhov on 04/08/14.
//  Copyright (c) 2014 Andrey Terekhov. All rights reserved.
//
#define _CRT_SECURE_NO_WARNINGS
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Defs.h"
#include "uniscanner.h"

/* Find a symbol in a buffer */
static int
find_symbol(const char *buffer, unsigned char symbol)
{
    const char *tmp = strchr(buffer, symbol);
    if (tmp == NULL)
        return -1;

    return tmp - buffer;
}

/* In-memory UTF8 scanner */
int
io_mem_getnext(universal_scanner_options *opts)
{
    unsigned char firstchar, secondchar;
    int           ret;
    int           pos;

    if (sscanf(&opts->ptr[opts->pos], "%c%n", &firstchar, &pos) != 1)
        return EOF;

    /* We must find the symbol because we already did in sscanf() call */
    opts->pos += pos;

    if ((firstchar & /*0b11100000*/ 0xE0) == /*0b11000000*/ 0xC0)
    {
        if (sscanf(&opts->ptr[opts->pos], "%c%n", &secondchar, &pos) != 1)
            return EOF;

        opts->pos += pos;

        ret = ((int)(firstchar & /*0b11111*/ 0x1F)) << 6 |
            (secondchar & /*0b111111*/ 0x3F);
    }
    else
    {
        ret = firstchar;
    }

    if (ret == '\r')
        return io_mem_getnext(opts);

    return ret;
}

int
io_mem_scanf(universal_scanner_options *opts, const char *fmt, va_list args)
{
    UNUSED(opts);
    UNUSED(fmt);
    UNUSED(args);

    fprintf(stderr, "mem scanf not implemented\n");
    exit(4);
}

scanner_desc scanner_mem = { IO_SOURCE_MEM, io_mem_getnext, io_mem_scanf };