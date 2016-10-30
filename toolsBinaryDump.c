/*
 * binaryDump.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum toolsApi_type
{
    toolsNG = -1,
    toolsOK = 0
} ToolsApi_t, *pToolsApi_t;

ToolsApi_t binaryDump(void* buffer, ssize_t size, int width);

#define DISP_WIDTH_MAX 16
#define DISP_ADDR_SIZE  4
#define DISP_DATA_SIZE  1

#define DISP_BUFF_MAX   4096

ToolsApi_t binaryDump(void* buffer, ssize_t size, int width)
{
    ToolsApi_t rc = toolsNG;
    char* disp = NULL;
    unsigned int cnt = 0;
    unsigned char* src_byte = NULL;
    int dp = 0;

    disp = (char *)calloc( DISP_BUFF_MAX, sizeof(char));
    
    if((buffer != NULL) && (width <= DISP_WIDTH_MAX) && (disp != NULL))
    {
        src_byte = (unsigned char*)buffer;

        /* add address */
        sprintf(&disp[dp], "%04x ", cnt);
        dp = strnlen(disp, DISP_BUFF_MAX);

        for(cnt = 0; cnt < size; cnt++)
        {
            sprintf(&disp[dp], "%02x ", src_byte[cnt]);
            dp = strnlen(disp, DISP_BUFF_MAX);
            if((cnt + 1) % width == 0)
            {
                /* add carriage return */
                sprintf(&disp[dp], "\n");
                dp = strnlen(disp, DISP_BUFF_MAX);
                /* add address */
                sprintf(&disp[dp], "%04x ", cnt + 1);
                dp = strnlen(disp, DISP_BUFF_MAX);
            }
        }

        fprintf(stderr, "%s\n", disp);
        rc = toolsOK;
        free(disp);
    }

    return rc;
}

int main(int argc, char **argv)
{
    unsigned char buffer[] = {  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
                            };

    ToolsApi_t rc = binaryDump(buffer, sizeof(buffer), 16);

    printf("binaryDump return = %d\n", rc);

    return 0;
}

