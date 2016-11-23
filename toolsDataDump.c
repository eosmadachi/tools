/*
 * dataDump.c
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

#include "toolsDataDump.h"

void createTitle(char *title, char *strFile, int tsize, int size, toolsDataType_t dataType, int width)
{
    char dataFmt[24] = {0};
    int i = 0, len = 0, inc = 0;
    
    if(size < 0x10000)
    {
        sprintf(title, "%saddr: ", strFile);
    }else{
        sprintf(title, "%s  addr  : ", strFile);
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
    memset(&title[len], '-', len - 1);
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
        titleSize = strlen("aaaa: ") + strlen("¥n");
    }else{
        titleSize = strlen("aaaaaaaa: ") + strlen("¥n");
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

toolsApi_t dataDump(p_toolsData_t tData)
{
    static char fname[PATH_MAX] = "";
    toolsApi_t rc = toolsOK;
    char *disp = NULL;
    int cnt = 0;
    int lineCnt = 0;
    int dp = 0;
    int aSize = 0;
    char *title = NULL;
    
    aSize = calBufferSize(tData);
    if(aSize == -1)
    {
        fprintf(stderr, "Illegal parameter error : Data type¥n");
        rc = toolsIllegalParameterError;
    }
    /* insert file name */
    if(tData->insertFName)
    {
        sprintf(fname, "%s: ", tData->file);
    }

    if(rc == toolsOK)
    {
        /* display offset title */
        title = (char *)calloc(strlen(fname) + tData->lineSize, 2); /* offset + "-" */
        if(title != NULL)
        {
            if((tData->title > 0) || (tData->title == OFFSET_ONLY_TOP))
            {        
                createTitle(title, fname, tData->lineSize * 2, tData->size, tData->type, tData->width);
            }
        }else{
            rc = toolsGetResourceError;
        }
    }

    if(rc == toolsOK)
    {
        /* allocate line buffer */
        disp = (char *)calloc( tData->lineSize, sizeof(char));
        if(disp != NULL)
        {
            cnt = 0;
            if(tData->title == OFFSET_ONLY_TOP)
            {
                fprintf(stdout, "%s\n", title);
            }

            while(cnt < tData->dataSize)
            {
                if(cnt % tData->width == 0)
                {
                    dp = 0;         /* clear of append pointer */
                    /* title display */
                    if(tData->title > 0)
                    {
                        if((lineCnt % tData->title == 0) && (cnt < tData->dataSize))
                        {
                            fprintf(stdout, "\n%s\n", title);
                        } 
                    }

                    /* add address */
                    if(tData->size < 0x10000)
                    {
                        sprintf(&disp[dp], "%04x: ", cnt);
                    }else{
                        sprintf(&disp[dp], "%08x: ", cnt);
                    }
                    dp = strnlen(disp, aSize);
                }

                /* add data */
                setFormatData(tData, disp, dp, &cnt);
                dp = strnlen(disp, aSize);

                if((cnt % tData->width == 0) && (cnt <= tData->dataSize))
                {
                    /* add carriage return */
                    sprintf(&disp[dp], "\n");
                    /* display line  */
                    fprintf(stdout, "%s%s", fname, disp);

                    /* inclement line count */
                    lineCnt++;
                }
            }
            free(title);
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

    while((result=getopt(argc, argv, "fht::w:d:")) != -1)
    {
        switch(result)
        {
        /* If option of argument has the value, the value is stored to "optarg" */
        case 'd':   /* data word size */
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

        case 'f':
            tData->insertFName = 1;
            break;
        case 'h':
            fprintf(stdout, "Binary Dump Tool \n");
            fprintf(stdout, "usage: toolsdataDump [-d data type] [-w line width ] [-t offset title] [file ] \n");
            fprintf(stdout, "       -d data type         -s8:8bit -s16:16bit -s32:32bit -s64:64bit \n");
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

toolsApi_t getDataAndDump(p_toolsData_t tData)
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
                    if((rc = dataDump(tData)) != toolsOK)
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
    tData.title = -1;
    tData.insertFName = 0;
    tData.type = dspType8;
    rc = argumentParsing(argc, argv, &tData);
    if(rc == toolsOK)
    {
        if(tData.file == NULL)
        {
            /* for multi files */
            tData.file = readline;
            while(fgets(tData.file, PATH_MAX, stdin) != NULL)
            {
                rc = getDataAndDump(&tData);
                if(rc != toolsOK)
                {
                    break;
                }
            }
        }else{
            /* for single file  */
            rc = getDataAndDump(&tData);
        }
    }

    return rc;
}
