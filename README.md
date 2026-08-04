The goal of this project is to make a playable minesweeper with the largest possible grid (reduce the memory footprint of the grid as much as possible). This is currently V3 of this project. 

V1 was one of the first things I ever made with c++ and SDL was used to render it I managed to get it down to 1 byte per cell however the process of rendering per pixel and using SDL draw functions was costing way too much performance and it would leak memory like crazy somewhere in SDL. 

V2 used the same 1 byte per cell logic but instead of SDL draw functions I rendered using the minefield as a texture with OpenGL however this one wasn't playable due to only rendering a revealed minefield.

The idea behind V3 is to use the new architecture I thought up where you use 2bits per cell this reduces the memory footprint by 4x. The reason I didn't consider doing this before is the fact that generating numbers becomes much more difficult because you have to loop every non bomb cell rather than looping each bomb cell however I plan on doing this on the GPU and I believe its possible within a reasonable amount of performance.

A bonus of the new strategy is bomb generation can now be easily multi threaded since you do not have to loop around cells making it easy to just split the grid and give each section to a thread without having to worry about race conditions when looping around the bomb placement. This is massive because the generation was by far the biggest bottleneck in V2.

The current rendering plan is to generate the bit packed minefield -> split it into chunks -> use a compute shader to convert the relevant visible chunks into the same 1 byte format as used in previous versions -> use a variation of the V2 shader to render those to the screen.

To make it playable I have decided to use raylib while I'll still be using opengl calls (hopefully rl calls if I can) it means I don't have to manage all the annoying stuff. I do have my own shader loader which I used for V2 but that is for OpenGL 3.3 and I need 4.3 to use compute shaders. I was using GLFW for V2 and raylib wraps that making window stuff even easier. Input becomes easy. Raylib provides a text back end meaning drawing text on menus and things is easier.
