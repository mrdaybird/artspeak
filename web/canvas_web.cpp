#include <array>
#include <cmath>
#include <fmt/core.h>
#include <stdio.h>

#include <raylib.h>
#include <emscripten/emscripten.h>
#include "lang.hpp"

Lang lang{true, true};

//clang-format off
EM_JS(void, init, (), {
	window.removeEventListener("keydown",GLFW.onKeydown, true);
	var div = document.createElement("div");
	div.setAttribute("class", "interpreter");

	var span = document.createElement("span");
	span.style.color = "rgb(0,166,255)";
	span.textContent = ">> ";
	div.appendChild(span);

	var input = document.createElement("input");
	input.setAttribute("class", "code");
	input.setAttribute("spellcheck", "false");
	input.setAttribute("value", "sin((x+y)/4+t)");
	input.autofocus = false;
	input.readOnly = true;
	div.appendChild(input);
	var consoleDiv = document.querySelector(".repl");
	consoleDiv.appendChild(div);
	
	Module.ccall("compile", 'void', ['number'], [Module.stringToNewUTF8(input.value)]);
});	

EM_JS(void, addInputNode, (), {
	// task 2 start : create the interpreter box
	var div = document.createElement("div");
	div.setAttribute("class", "interpreter");

	var span = document.createElement("span");
	span.style.color = "rgb(0,166,255)";
	span.textContent = ">> ";
	div.appendChild(span);

	var input = document.createElement("input");
	input.setAttribute("class", "code");
	input.setAttribute("spellcheck", "false");
	input.setAttribute("id", "active");
	input.addEventListener("keydown", function(e) {
		if(e.keyCode == 13) passValueToInterpreter();
	 });

	div.appendChild(input);
	var consoleDiv = document.querySelector(".repl");
	consoleDiv.appendChild(div);

	var inputBox = document.getElementById("active");
	active.focus();
});

//clang-format off
EM_JS(void, passValueToInterpreter, (), {
	// task 1 start: make the current input readonly
	var inputBox = document.getElementById("active");
	inputBox.autofocus = false;
	inputBox.id = "";
	inputBox.readOnly = true;
	console.log("Code from inputBox:", inputBox.value);
	// task 1 end;
	Module.ccall("compile", 'void', ['number'], [Module.stringToNewUTF8(inputBox.value)]);
});

EM_JS(void, showResult, (const char *result), {
	var p = document.createElement("p");
	p.setAttribute("class", "result");
	p.textContent = Module.UTF8ToString(result); // TODO add the actual result;

	var consoleDiv = document.querySelector(".repl");
	consoleDiv.appendChild(p);

	addInputNode();
});

extern "C"
{
	void compile(char *source)
	{
		lang.compileToBytecode(source);
		addInputNode();
	}
}
const int sideLength = 480;
const int widthPadding = 0, heightPadding = 0;
const int screenWidth = sideLength + widthPadding, screenHeight = sideLength + heightPadding;

const int atomSize = 20; // factor of sideLength
const double ballRadius = atomSize / 2 - 1;
const int numX = sideLength / atomSize, numY = sideLength / atomSize;

double values[numX][numY];

void Update()
{
	// Update Stuff
	double t = GetTime();
	for (int y = 0; y < numY; y++)
	{
		for (int x = 0; x < numX; x++)
		{
			int i = y * numX + x;
			double temp = lang.getValue(t, i, x, y);
			if (lang.allOK())
				values[x][y] = temp;
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

			DrawCircle(widthPadding / 2 + x * atomSize + atomSize / 2,
					   heightPadding / 2 + y * atomSize + atomSize / 2, radius, color);
		}
	}

	EndDrawing();
}
int main()
{
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(screenWidth, screenHeight, "Canvas - artspeak");
	SetTargetFPS(60);
	for (int y = 0; y < numY; y++)
		for (int x = 0; x < numX; x++)
			values[x][y] = 0;
	init();
	emscripten_set_main_loop(Update, 0, 1);
}
