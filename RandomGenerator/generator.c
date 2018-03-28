#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv) {
    
    if (argc > 2)
    {
        int passport = atoi(argv[1]);
        int n = atoi(argv[2]);
        int r;
    
        srand(passport);
        for(int i = 0; i < passport; i++)
        {
            r = rand();
        }
    
        printf("Your var: %d\n", (r % n) + 1);      
    }
    else
    {
        printf("Wrong parameters!");
    }
    
    
    return (0);
}
