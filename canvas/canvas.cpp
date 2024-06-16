#include <raylib.h>

#include <array>
#include <cmath>
#include <fmt/core.h>
#include <stdio.h>
#include <thread>

#include "lang.hpp"

void canvasREPL(Lang &lang){
    char line[1024];
    for(;;){
        fmt::print(">> ");
        if(!fgets(line, sizeof(line), stdin)){
            fmt::println("");
            break;
        }

        lang.compileToBytecode(line);
    }
}

int main()
{
    const int sideLength = 480;
    const int screenWidth = sideLength, screenHeight = sideLength;

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

                DrawCircle(x * atomSize + atomSize / 2, y * atomSize + atomSize / 2, radius, color);
            }
        }

        EndDrawing();
    }
}
