#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>

char resolved_path[PATH_MAX];

void call_realpath (char * argv0)
{ 
    if (realpath (argv0, resolved_path) == 0) 
    { 
	    fprintf (stderr, "realpath() error: %s\n", strerror (errno));
        exit(1);
    }
}

int getSize(char* fname)
{
    struct stat st;
    if (lstat (fname, &st) == -1) {
        fprintf (stderr, "%s: lstat() error: %s\n", resolved_path, strerror(errno));
        exit (1);
    }
    return st.st_size;
}

int isDir(char* fname)
{
    struct stat st;
    if (lstat (fname, &st) == -1) {
        fprintf (stderr, "%s: lstat() error: %s\n", resolved_path, strerror(errno));
        exit (1);
    }
    return S_ISDIR(st.st_mode);
}

int notDots(char* fname)
{
    return !(!strcmp(fname, ".") || !strcmp(fname, ".."));
}

char* getFullPath(char* fname, char* path)
{
    char* fullPath = malloc(strlen(fname) + strlen(path) + 2);
    sprintf(fullPath, "%s/%s", path, fname);
    return fullPath;
}

char* currDir;
FILE* outputFile;

void traversal(char* path)
{
    DIR* dir; 
    dir = opendir(path);
    if (dir == NULL) 
    {
        fprintf (stderr, "%s: opendir() error: %s\n", resolved_path, strerror(errno));
        exit(1);
    }

    struct dirent* entry;
    int files_amount = 0;
    int files_size = 0;
    int max = 0;
    char* maxFile = malloc(1);
    maxFile = "\0";
    
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry == NULL)
        {
            fprintf (stderr, "%s: readdir() error: %s\n", resolved_path, strerror(errno));
            exit(1);  
        }
        char* fileName = entry->d_name;
        if (notDots(fileName))
        {
            char* fullPath = getFullPath(fileName, path);
            if (isDir(fullPath))
            {   
                free(currDir);
                currDir = (char*) malloc(strlen(fullPath) + 1); 
                strcpy(currDir, fullPath);

                traversal(fullPath); 

                free(currDir);
                currDir = (char*) malloc(strlen(path) + 1);
                strcpy(currDir, path);
            } 
            else 
            {
                int currentSize = getSize(fullPath);
                files_amount++;
                files_size += currentSize;
                if (currentSize > max)
                {
                    maxFile = fileName;
                    max = currentSize;
                }
            } 
            free(fullPath);
        }    
    }    

    printf("%s: %d %d %s\n",currDir, files_amount, files_size, maxFile);
    fprintf(outputFile,"%s: %d %d %s\n",currDir, files_amount, files_size, maxFile); 
    
    closedir(dir);
}


int main(int args, char **argv)
{
    if (args < 3)
    {
        printf("Not enough arguments!\n");
        return 1;
    }
    currDir = malloc(1);

    call_realpath(argv[0]);
    char* initPath = argv[1]; 
    outputFile = fopen(argv[2], "w+");

    traversal(initPath);

    fclose(outputFile);
}
