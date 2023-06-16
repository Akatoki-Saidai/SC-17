#include <stdio.h>
#include "client.hpp"
int main(void){
    const char *address = "127.0.0.1";
    int port = 6969;
    setup(address,port);
    client("Hello World");
    client("Good Bye");
    return 0;
}
