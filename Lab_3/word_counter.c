#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>


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
    FILE* f;
    f = fopen(fname, "r");

    if (f == NULL)
    {
        fprintf(stderr, "%s: fopen() error: %s\n", resolved_path, strerror(errno));
        exit(1);
    }
    fseek(f, 0L, SEEK_END);
    int size = ftell(f);
    fclose(f);
    return size;
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


int isRegular(char* fname)
{
    struct stat st;
    if (lstat (fname, &st) == -1) {
        fprintf (stderr, "%s: lstat() error: %s\n", resolved_path, strerror(errno));
        exit (1);
    }
    return S_ISREG(st.st_mode);
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


int getWordCount(char* fname)
{
    int CHUNK = SHRT_MAX;
    char buf[CHUNK];

    FILE *file;
    size_t nread;

    file = fopen(fname, "r");
    
    int tot_words = 0;  
    int in_space = 1;
    
    if (file) 
    {
        while ((nread = fread(buf, 1, CHUNK, file)) > 0)
        {
            for(int i = 0; i < nread; i++)
            {      
                if (isspace(buf[i])) 
                {
                    in_space = 1;
                } 
                else 
                {
                    tot_words += in_space;
                    in_space = 0;
                }

            }
        }
        if (ferror(file)) 
        {
            fprintf (stderr, "%s: fread() error: %s\n", resolved_path, strerror(errno));
            exit(1);
        }

        fclose(file);
        return tot_words;
    } 
    else
    {
        
        fprintf (stderr, "%s: fopen() error: %s\n", resolved_path, strerror(errno));
        exit(1);    
    } 
}


pid_t pid;
int maxAmount = 0;
int currAmount = 0;
void traversal(char* path)
{
         
    DIR* dir; 
    dir = opendir(path);

    struct dirent* entry;

    if (dir == NULL) 
    {
        fprintf (stderr, "%s: opendir() error: %s\n", resolved_path, strerror(errno));
        exit(1);
    }
    while ((entry = readdir(dir)) != NULL)
    {         
        if (entry == NULL)
        {
            fprintf (stderr, "%s: readdir() error: %s\n", resolved_path, strerror(errno));
            exit(1);  
        }
        char* fileName = entry->d_name;
        char* fullPath = getFullPath(fileName, path);
        if (notDots(fileName))
        {
            if (isDir(fullPath))
            {   
                traversal(fullPath); 
            } 
            else if (isRegular(fullPath))
            {

                if (currAmount < maxAmount) 
                {
                    currAmount++;
                    pid = fork();
                }    

                if (pid == 0)
                {
                    int amount = getWordCount(fullPath);
                    printf("%d %s %d %d\n", getpid(),fullPath, getSize(fullPath), amount);
                    exit(0);
                }

                if (currAmount == maxAmount)
                {
                    int status = 0;
                    wait(&status);
                    currAmount--;  
                }
            } 
            free(fullPath);
        }    

    }    
    closedir(dir);
}


int main(int args, char **argv)
{
    if (args < 3)
    {
        printf("Not enough arguments!\n");
        return 1;
    }
    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);

    call_realpath(argv[0]);
    char* initPath = argv[1];
    maxAmount = atoi(argv[2]) - 1;  
    
    traversal(initPath);

    for (int i = 0; i < currAmount; i++)
    {
        int status;
        wait(&status);
    }

    gettimeofday(&tv2, NULL);
    //printf ("Total time = %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
}