#include "lang.hpp"

#include <fmt/core.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void repl(){
    char line[1024];
    Lang lang{true, true};
    for(;;){
        fmt::print("> ");
        if(!fgets(line, sizeof(line), stdin)){
            fmt::println("");
            break;
        }        

        lang.interpret(line);
    }
}

int main(int argc, const char* argv[]){
    if(argc == 1){
        repl();
    }else{
        fmt::println("Unexpected args!");
        exit(64);
    }
}
