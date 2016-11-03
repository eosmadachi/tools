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

#include "toolsBinaryDump.h"

int calBufferSize(ssize_t size, toolsDataType_t dataType, int width)
{
    int aSize = -1;

    if(size < 0x10000)
    {
        aSize += size / width * sizeof("aaaa ");
    }else{
        aSize += size / width * sizeof("aaaaaaaa ");
    }

    switch(dataType)
    {
        case dspType8:
            aSize += size * sizeof("dd ");
            break;
        case dspType16:
            aSize += (size/2) * sizeof("dddd ");
            break;
        case dspType32:
            aSize += (size/4) * sizeof("dddddddd ");
            break;
        case dspType64:
            aSize += (size/8) * sizeof("dddddddddddddddd ");
            break;
        default:
            aSize = -1;
            break;
    }
    
    return aSize;
}

/*
 * 
 * name: binaryDump API
 * @param[in] *buffer top of the binary data.  
 * @param[in] size    length of binary data.
 * @param[in] width   display size of binary data.
 * @return toolsApi_t result of function
 * 
 */
toolsApi_t binaryDump(void* buffer, ssize_t size, toolsDataType_t dataType, int width)
{
    toolsApi_t rc = toolsOK;
    char* disp = NULL;
    unsigned int cnt = 0;
    unsigned char* src_byte = NULL;
    int dp = 0;
    int aSize = 0;
    
    aSize = calBufferSize(size, dataType, width);
    if(aSize == -1)
    {
        fprintf(stderr, "Illegal parameter error : Data type¥n");
        rc = toolsIllegalParameterError;
    }
    
    if(rc == toolsOK)
    {
        disp = (char *)calloc( aSize, sizeof(char));
        
        if((buffer != NULL) && (disp != NULL))
        {
            src_byte = (unsigned char*)buffer;

            for(cnt = 0; cnt < size; cnt++)
            {
                if(cnt % width == 0)
                {
                    /* add address */
                    sprintf(&disp[dp], "%04x ", cnt);
                    dp = strnlen(disp, aSize);
                }

                sprintf(&disp[dp], "%02x ", src_byte[cnt]);
                dp = strnlen(disp, aSize);
                if((cnt + 1) % width == 0)
                {
                    /* add carriage return */
                    sprintf(&disp[dp], "\n");
                    dp = strnlen(disp, aSize);
                }
            }

            fprintf(stderr, "%s\n", disp);
            rc = toolsOK;
            free(disp);
        }
    }

    return rc;
}

int main(int argc, char **argv)
{
    unsigned char buffer[] = {  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
                            };
    static char readline[PATH_MAX] = {0};

    /* ファイルの終端まで文字を読み取り表示する */
    while ( fgets(readline, PATH_MAX, stdin) != NULL ) {
        printf("%s\n", readline);
    }

    toolsApi_t rc = binaryDump(buffer, sizeof(buffer), dspType8, 16);

    printf("binaryDump return = %d\n", rc);

    return 0;
}

