#include <raylib.h>

#include <array>
#include <cmath>
#include <fmt/core.h>
#include <stdio.h>
#include <thread>

#include "lang.hpp"

// from example core_custom_logging.c
void logToFileCallback(int msgType, const char *text, va_list args)
{
    FILE *fptr = fopen("log.txt", "w");
    if(fptr == NULL){
        fmt::print(stderr, "Could not open file 'log.txt'. ");
        return;
    }

    char timeStr[64] = { 0 };
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
    fprintf(fptr, "[%s] ", timeStr);

    switch (msgType)
    {
        case LOG_INFO: fprintf(fptr, "[INFO] : "); break;
        case LOG_ERROR: fprintf(fptr, "[ERROR]: "); break;
        case LOG_WARNING: fprintf(fptr, "[WARN] : "); break;
        case LOG_DEBUG: fprintf(fptr, "[DEBUG]: "); break;
        default: break;
    }

    vfprintf(fptr, text, args);
    fprintf(fptr, "\n");
}

int wantToCompile = 0;

void canvasREPL(Lang &lang){
    // sample startup code -->
    fmt::println(">> sin((x-y)/4 + t)");
    lang.compileToBytecode("sin((x-y)/4 + t)");
    
    // start repl -->
    char line[1024];
    for(;;){
        fmt::print(">> ");
        if(!fgets(line, sizeof(line), stdin)){
            fmt::println("");
            break;
        }
        wantToCompile = 1; // signal the wish to compile 
        while(wantToCompile != 2); // wait for main thread to give OK;
        lang.compileToBytecode(line);
        wantToCompile = 0; // signal compile done.
    }
}

int main()
{
    const int sideLength = 480;
    const int widthPadding = 220, heightPadding = 220;
    const int screenWidth = sideLength + widthPadding, screenHeight = sideLength + heightPadding;

    SetTraceLogCallback(logToFileCallback);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Canvas - artspeak");
    SetTargetFPS(60);

    const int atomSize = 20; // factor of sideLength
    const double ballRadius = atomSize / 2 - 1;
    const int numX = sideLength / atomSize, numY = sideLength / atomSize;

    // drawFn => (t, i, x, y)
    // x,y,i for each atom is known before hand, but time can only be known
    // if it running

    double values[numX][numY];
    for(int y = 0; y < numY; y++)
        for(int x = 0; x < numX; x++)
            values[x][y] = 0;

    Lang lang{true, true};

    std::thread first(canvasREPL, std::ref(lang));
    first.detach();

    while (!WindowShouldClose())
    {
        // Update Stuff
        if(wantToCompile == 0){ // semaphore style synchronization
            double t = GetTime();
            for (int y = 0; y < numY; y++)
            {
                for (int x = 0; x < numX; x++)
                {
                    int i = y * numX + x;
                    double temp = lang.getValue(t, i, x, y);
                    if(lang.allOK()) values[x][y] = temp;
                }
            }
        }else if(wantToCompile == 1){
            wantToCompile = 2; // hand over control to repl thread
        }
        // Draw Stuff
        BeginDrawing();
        ClearBackground(BLACK);

        for (int y = 0; y < numY; y++)
        {
            for (int x = 0; x < numX; x++)
            {
                double fn_value = values[x][y];
                fn_value = fn_value > 1 ? 1 : (fn_value < -1 ? -1 : fn_value);
                // parameters of the circle
                double radius = ballRadius * std::abs(fn_value);
                Color color = fn_value > 0 ? DARKBLUE : SKYBLUE;

                DrawCircle(widthPadding/2 + x * atomSize + atomSize / 2,
                            heightPadding/2 + y * atomSize + atomSize / 2, radius, color);
            }
        }

        EndDrawing();
    }
}
