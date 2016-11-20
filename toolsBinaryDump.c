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
    char dataFmt[24] = {0};
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
            strcpy(dataFmt, OFFSET_FMT_08); // 00
            inc = 1;
            break;
        case dspType16:
            strcpy(dataFmt, OFFSET_FMT_16); // _00_
            inc = 2;
            break;
        case dspType32:
            strcpy(dataFmt, OFFSET_FMT_32); // ___00___
            inc = 4;
            break;
        case dspType64:
            strcpy(dataFmt, OFFSET_FMT_64); // _______00_______
            inc = 8;
            break;
        default:
            strcpy(dataFmt, OFFSET_FMT_08);
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

int calBufferSize(p_toolsData_t tData)
{
    int aSize = 0;
    int lineCnt = 0;
    int wordSize = 0, titleSize = 0;
    
    /* calculate line count */
    if(tData->size % tData->width > 0) lineCnt += 1;
    lineCnt += tData->size / tData->width;

    /* calculate address buffer size (include CR) */
    if(tData->size < 0x10000)
    {
        titleSize = strlen("aaaa ") + strlen("¥n");
    }else{
        titleSize = strlen("aaaaaaaa ") + strlen("¥n");
    }
    aSize = lineCnt * titleSize;

    /* calculate word count(full data size on line) */
    tData->dataSize = lineCnt * tData->width;

    /* calculate data buffer size */
    switch(tData->type)
    {
        case dspType8:
            wordSize = strlen("dd ");
            aSize += tData->dataSize * wordSize;
            break;
        case dspType16:
            wordSize = strlen("dddd ");
            aSize += (tData->dataSize / 2) * wordSize;
            break;
        case dspType32:
            wordSize = strlen("dddddddd ");
            aSize += (tData->dataSize / 4) * wordSize;
            break;
        case dspType64:
            wordSize = strlen("dddddddddddddddd ");
            aSize += (tData->dataSize / 8) * wordSize;
            break;
        default:
            aSize = -1;
            break;
    }
    tData->lineSize = (wordSize * tData->width) + titleSize;
    
    return aSize;
}

void setFormatData(p_toolsData_t tData, char *disp, int dp, int *cnt)
{
    uint8_t *buffer = tData->buffer;

    switch(tData->type)
    {
        case dspType8:
            if(*cnt < tData->size)
            {
                sprintf(&disp[dp], "%02"PRIx8" ", *((uint8_t *)&(buffer[*cnt])));
            }else{
                sprintf(&disp[dp], ".. ");
            }
            *cnt += sizeof(uint8_t);
            break;

        case dspType16:
            if(*cnt < tData->size)
            {
                sprintf(&disp[dp], "%04"PRIx16" ", *((uint16_t *)&(buffer[*cnt])));
            }else{
                sprintf(&disp[dp], ".... ");
            }
            *cnt += sizeof(uint16_t);
            break;

        case dspType32:
            if(*cnt < tData->size)
            {
                sprintf(&disp[dp], "%08"PRIx32" ", *((uint32_t *)&(buffer[*cnt])));
            }else{
                sprintf(&disp[dp], "........ ");
            }
            *cnt += sizeof(uint32_t);
            break;

        case dspType64:
            if(*cnt < tData->size)
            {
                sprintf(&disp[dp], "%016"PRIx64" ", *((uint64_t *)&(buffer[*cnt])));
            }else{
                sprintf(&disp[dp], "................ ");
            }
            *cnt += sizeof(uint64_t);
            break;

        default:
            fprintf(stderr, "Display type error(type %d) \n", tData->type);
            break;
    }
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
toolsApi_t binaryDump(p_toolsData_t tData)
{
    toolsApi_t rc = toolsOK;
    char *disp = NULL;
    int cnt = 0;
    int dp = 0;
    int aSize = 0;
    char *title = NULL;
    
    aSize = calBufferSize(tData);
    if(aSize == -1)
    {
        fprintf(stderr, "Illegal parameter error : Data type¥n");
        rc = toolsIllegalParameterError;
    }
    
    if(rc == toolsOK)
    {
        /* display offset title */
        title = (char *)calloc(tData->lineSize, 2);
        if(title != NULL)
        {
            if(tData->title > 0)
            {        
                createTitle(title, tData->lineSize * 2, tData->size, tData->type, tData->width);
                fprintf(stdout, "%s\n", title);
            }
            free(title);
        }else{
            rc = toolsGetResourceError;
        }
    }

    if(rc == toolsOK)
    {
        /* display data */
        disp = (char *)calloc( aSize+8, sizeof(char));
        if(disp != NULL)
        {
            cnt = 0;
            while(cnt < tData->dataSize)
            {
                if(cnt % tData->width == 0)
                {
                    /* add address */
                    if(tData->size < 0x10000)
                    {
                        sprintf(&disp[dp], "%04x ", cnt);
                    }else{
                        sprintf(&disp[dp], "%08x ", cnt);
                    }
                    dp = strnlen(disp, aSize);
                }

                setFormatData(tData, disp, dp, &cnt);
                dp = strnlen(disp, aSize);

                if((cnt % tData->width == 0) && (cnt != tData->size))
                {
                    /* add carriage return */
                    sprintf(&disp[dp], "\n");
                    dp = strnlen(disp, aSize);
                }
            }

            fprintf(stdout, "%s\n", disp);
            free(disp);
        }

        fprintf(stdout, "\n");
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
            fprintf(stderr, "\n");
            fprintf(stderr, "\n");
        }
    }else{
        fprintf(stderr, "Not regular file (mode:%x)\n", statBuf.st_mode);
        fprintf(stderr, "\n");
        fprintf(stderr, "\n");
    }

    return rc;
}

int argumentParsing(int argc, char **argv, toolsData_t *tData)
{
    int rc = toolsOK;
    int result;

    while((result=getopt(argc, argv, "ht::w:s:")) != -1)
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

        case 'h':
            fprintf(stdout, "Binary Dump Tool \n");
            fprintf(stdout, "usage: toolsBinaryDump [-s data type] [-w line width ] [-t offset title] [file ] \n");
            fprintf(stdout, "       -s data type         -s8:8bit -s16:16bit -s32:32bit -s64:64bit \n");
            fprintf(stdout, "       -w line width        byte size for line width\n");
            fprintf(stdout, "       -t offset title      display an offset title -t0:no title\n");
            rc = toolsNG;
            break;

        case '?':
            /* If there is not value that is specified on the getopt, result is "?" */
            fprintf(stdout, "There is illegal option on the command line.\n");
            break;
        }
    }

    if(rc == toolsOK)
    {
        /* Handle argument without option */
        for(; optind < argc; optind++)
        {
            tData->file = argv[optind];
            break;
        }
    }
  
    return rc; 
}

toolsApi_t binaryDumpLoop(p_toolsData_t tData)
{
    char *fnp   = NULL;
    FILE *fp    = NULL;
    toolsApi_t rc = toolsOK;

    fnp = strchr(tData->file, '\n');
    if(fnp != NULL)
    {
        *fnp = '\0';
    }

    fprintf(stdout, "<< File Name: %s >>\n", tData->file);
    fp = fopen(tData->file, "r");
    if(fp != NULL)
    {
        if((tData->size = getFileState(fp)) != 0)
        {
            tData->buffer = (unsigned char *)calloc(tData->size + 8, sizeof(char));
            if(tData->buffer != NULL)
            {
                if(fread(tData->buffer, sizeof(char), tData->size, fp) > 0)
                {
                    if((rc = binaryDump(tData)) != toolsOK)
                    {
                        fprintf(stderr, "Dump error = %d\n", rc);
                    }
                }
                else
                {
                    fprintf(stderr, "File read error\n");
                }
                free(tData->buffer);
            }else{
                fprintf(stderr, "Memory allocation error(%d)\n", errno);
            }
        }
        fclose(fp);
    }else{
        fprintf(stderr, "File open error(%d)\n", errno);
    }
    
    return rc;
}

int main(int argc, char *argv[])
{
    toolsData_t tData = {0};
    char readline[PATH_MAX] = {0};
    int rc = 0;

    tData.width = 16;
    tData.title = 1;
    tData.type = dspType8;
    rc = argumentParsing(argc, argv, &tData);
    if(rc == toolsOK)
    {
        if(tData.file == NULL)
        {
            tData.file = readline;
            while(fgets(tData.file, PATH_MAX, stdin) != NULL)
            {
                rc = binaryDumpLoop(&tData);
                if(rc != toolsOK)
                {
                    break;
                }
            }
        }else{
            rc = binaryDumpLoop(&tData);
        }
    }

    return rc;
}

