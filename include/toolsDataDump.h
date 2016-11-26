/*
 * toolsBinaryDump.h
 * 
 * Copyright 2016 Masaaki Adachi <web.adachi@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#ifndef __TOOLS_BINARY_DUMP_H__
#define __TOOLS_BINARY_DUMP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>

/* enum */
typedef enum toolsApi
{
    toolsGetResourceError = -4,
    toolsIllegalParameterError = -3,
    toolsIllegalArgumentError = -2,
    toolsNG = -1,
    toolsOK = 0
} toolsApi_t, *p_toolsApi_t;

typedef enum toolsDataType
{
    dspType8 = 0,
    dspType16,
    dspType32,
    dspType64
} toolsDataType_t, *p_toolsDataType_t;

typedef struct toolsData
{
    uint8_t         *buffer;
    int             size;
    int             dataSize;
    int             lineSize;
    toolsDataType_t type;
    int             width;
    int             title;
    char            *file;
    int             insertFName;
} toolsData_t, *p_toolsData_t;

/* definition */
#define OFFSET_FMT_08    "%02x "                // dd + sp
#define OFFSET_FMT_16    " %02x  "              // _dd_ + sp
#define OFFSET_FMT_32    "   %02x    "          // ___dd___ + sp
#define OFFSET_FMT_64    "       %02x        "  // _______dd_______ + sp

#define OFFSET_ONLY_TOP     -1

/* prototype */
toolsApi_t binaryDump(p_toolsData_t tDate);


#endif /* #ifndef __TOOLS_BINARY_DUMP_H__ */
