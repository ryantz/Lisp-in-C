#include <stdio.h>

typedef struct{
    int length;
    int breadth;
    int height;
} cube;

int main(int argc, char** argv){
    cube c;
    c.length = 3;
    c.breadth = 2;
    c.height = 4;

    int volume = c.length * c.breadth * c.height;
    printf("%d\n", volume);
}