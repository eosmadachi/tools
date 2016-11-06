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

void createTitle(char *title, int tsize, int size, toolsDataType_t dataType, int width)
{
    char dataFmt[] = {"+%02x "};
    int i = 0, len = 0, inc = 0;
    
    if(size < 0x10000)
    {
        sprintf(title, "addr");
    }else{
        sprintf(title, "  addr  ");
    }
    len = strnlen(title, tsize);

    switch(dataType)
    {
        case dspType8:
            strcpy(dataFmt, " %02x");  
            inc = 1;
            break;
        case dspType16:
            strcpy(dataFmt, "  %02x");
            inc = 2;
            break;
        case dspType32:
            strcpy(dataFmt, "    %2x  ");
            inc = 4;
            break;
        case dspType64:
            strcpy(dataFmt, "      %2x    ");
            inc = 8;
            break;
        default:
            strcpy(dataFmt, " %02x");
            inc = 1;
            break;
    }

    for(i = 0; i < width; i += inc)
    {
        sprintf(&title[len], dataFmt, i);
        len = strnlen(title, tsize);
    }
    sprintf(&title[len], "\n");
    len = strnlen(title, tsize);
    memset(&title[len], '-', len-1);
}

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
toolsApi_t binaryDump(void* buffer, ssize_t size, p_toolsData_t tData)
{
    toolsApi_t rc = toolsOK;
    char* disp = NULL;
    unsigned int cnt = 0;
    unsigned char* srcByte = NULL;
    int dp = 0;
    int aSize = 0;
    char title[240] = {0};
    
    aSize = calBufferSize(size, tData->type, tData->width);
    if(aSize == -1)
    {
        fprintf(stderr, "Illegal parameter error : Data type¥n");
        rc = toolsIllegalParameterError;
    }
    
    if(tData->title > 0)
    {
        createTitle(title, sizeof(title), size, tData->type, tData->width);
        fprintf(stdout, "%s\n", title);
    }

    if(rc == toolsOK)
    {
        disp = (char *)calloc( aSize, sizeof(char));
        
        if((buffer != NULL) && (disp != NULL))
        {
            srcByte = (unsigned char*)buffer;

            for(cnt = 0; cnt < size; cnt++)
            {
                if(cnt % tData->width == 0)
                {
                    /* add address */
                    sprintf(&disp[dp], "%04x ", cnt);
                    dp = strnlen(disp, aSize);
                }

                sprintf(&disp[dp], "%02x ", srcByte[cnt]);
                dp = strnlen(disp, aSize);
                if((cnt + 1) % tData->width == 0)
                {
                    /* add carriage return */
                    sprintf(&disp[dp], "\n");
                    dp = strnlen(disp, aSize);
                }
            }

            fprintf(stdout, "%s\n", disp);
            rc = toolsOK;
            free(disp);
        }
    }

    return rc;
}

int getFileState(FILE *fp)
{
    int rc = 0;
    struct stat statBuf;

    fstat(fileno(fp), &statBuf);

    /* check mode(symbolic link and directory) */
    if((statBuf.st_mode & S_IFREG) != 0)
    {
        /* check mode(symbolic link and directory) */
        if((statBuf.st_mode & S_IFLNK) != S_IFLNK)
        {
            rc = statBuf.st_size;
        }else{
            fprintf(stderr, "Symbolic link (mode:%x)\n", statBuf.st_mode);
        }
    }else{
        fprintf(stderr, "Not regular file (mode:%x)\n", statBuf.st_mode);
    }

    return rc;
}

int argumentParsing(int argc, char **argv, toolsData_t *tData)
{
    int rc = toolsOK;
    int result;

    while((result=getopt(argc, argv, "t::w:s:")) != -1)
    {
        switch(result)
        {
        /* If option of argument has the value, the value is stored to "optarg" */
        case 's':   /* data word size */
            switch(strtol(optarg, NULL, 0))
            {
                case 8: tData->type  = dspType8;  break;
                case 16: tData->type = dspType16; break;
                case 32: tData->type = dspType32; break;
                case 64: tData->type = dspType64; break;
                default:
                    rc = toolsIllegalArgumentError;
                    fprintf(stdout, "There is illegal option parameter. (option s: %s)\n", optarg);
                    break;
            }
            break;
        case 't':   /* title */
            if(optarg != NULL)
            {
                tData->title = strtol(optarg, NULL, 0);
            }
            break;

        case 'w':   /* display line size */
            tData->width = strtol(optarg, NULL, 0);
            if(tData->width < 1 || tData->width > 256)
            {
                rc = toolsIllegalArgumentError;
                fprintf(stdout, "There is illegal option parameter. (option w: %s)\n", optarg);
            }
            break;
        case '?':
            /* If there is not value that is specified on the getopt, result is "?" */
            fprintf(stdout, "There is illegal option on the command line.\n");
            break;
        }
    }

    /* getoptはoptindに「次に処理すべき引数のインデクスを格納している. */
    /* ここではoptindを使用してオプションの値ではない値を処理する. */
    for(; optind < argc; optind++)
        fprintf(stdout, "%s \n", argv[optind]);
  
    return rc;
}

int main(int argc, char **argv)
{
    unsigned char* buffer = NULL;
    static char readline[PATH_MAX] = {0};
    FILE *fp = NULL;
    char *fnp = NULL;
    int length = 0;
    int rc = 0;
    toolsData_t tData = {0};

    tData.width = 16;
    tData.title = 1;
    tData.type = dspType8;
    rc = argumentParsing(argc, argv, &tData);
    if(rc != toolsOK)
    {
        return rc;
    }

    while (fgets(readline, PATH_MAX, stdin) != NULL)
    {
        fnp = strchr(readline, '\n');
        if(fnp != NULL)
        {
            *fnp = '\0';
        }

        fprintf(stdout, "File Name: %s\n", readline);
        fp = fopen(readline, "r");
        if(fp != NULL)
        {
            if((length = getFileState(fp)) != 0)
            {
                buffer = (unsigned char *)calloc(sizeof(char), length);
                if(buffer != NULL)
                {
                    if(fread(buffer, sizeof(char), length, fp) > 0)
                    {
                        toolsApi_t rc = binaryDump(buffer, length, &tData);
                        if(rc != toolsOK)
                        {
                            fprintf(stderr, "Dump error = %d\n", rc);
                        }
                    }
                    else
                    {
                        fprintf(stderr, "File read error\n");
                    }
                    free(buffer);
                }else{
                    fprintf(stderr, "Memory allocation error(%d)\n", errno);
                }
            }
            fclose(fp);
        }else{
            fprintf(stderr, "File open error(%d)\n", errno);
        }
    }

    return 0;
}

