#include "GameOfLife.h"
#include "Random.h"
#include "Texture.h"

bool GameOfLife::Initialize()
{
	// resize both bufferA and B to the number of cells (size.x * size.y)
	bufferA.resize(size.x * size.y);
	bufferB.resize(size.x * size.y);

	return false;
}

void GameOfLife::Step()
{
	// both buffers will switch places as read/write buffers
	    // references to read/write will refer to buffers that are currently being used for read/write
	
	frame++;

	// <buffer determined if frame is even or odd>
	std::vector<uint8_t>& readBuffer = (frame % 2 == 1) ? bufferA : bufferB; std::vector<uint8_t>& writeBuffer = (frame % 2 == 1) ? bufferB : bufferA;
	
    // update buffer
    // clear write buffer
    std::fill(writeBuffer.begin(), writeBuffer.end(), 0);

    for (int y = 0; y < size.y; y++)
    {
        for (int x = 0; x < size.x; x++)
        {
            int weight = 0; // the amount of cells alive

            // read surrounding 8 cells, accumulate weight
            // XXX
            weight += Read<uint8_t>(readBuffer, x - 1, y - 1);
            weight += Read<uint8_t>(readBuffer, x, y - 1);
            weight += Read<uint8_t>(readBuffer, x + 1, y - 1); 

            // X0X
            weight += Read<uint8_t>(readBuffer, x - 1, y);
            weight += Read<uint8_t>(readBuffer, x + 1, y);

            // XXX
            weight += Read<uint8_t>(readBuffer, x - 1, y + 1);
            weight += Read<uint8_t>(readBuffer, x, y + 1);
            weight += Read<uint8_t>(readBuffer, x + 1, y + 1);

            // game of life rules
             
            //1. Any live cell with fewer than two live neighbors dies, as if by underpopulation.
                // weight < 2 = DEATH
            //2. Any live cell with two or three live neighbors lives on to the next generation.
                // weight > 2 && weight < 3 = LIVES
            //3. Any live cell with more than three live neighbors dies, as if by overpopulation.
                // weight > 3 = DEATH
            //4. Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
                // weight == 3 = LIVES
            

            // if cell is alive, update
            if (Read<uint8_t>(readBuffer, x, y))
            {
                if (weight < 2) Write<uint8_t>(writeBuffer, x, y, 0); // DIES
                else if (weight == 2 || weight == 3) Write<uint8_t>(writeBuffer, x, y, 1); // LIVES
                else if (weight > 3) Write<uint8_t>(writeBuffer, x, y, 0); // DIES
            }
            else
            {
                // if cell is dead, update
                if (weight == 3) Write<uint8_t>(writeBuffer, x, y, 1); // LIVES
            }
        }
    }

    // convert buffer data format to color buffer
    std::transform((writeBuffer).begin(), (writeBuffer).end(), color_buffer.begin(), [](uint8_t v)
        {
            { return  v ? 0xffffffff : 0; }
        });

    // copy color buffer to texture
    texture->Copy(color_buffer);
}

void GameOfLife::KeyDown(SDL_Keycode keycode)
{
    if (keycode == SDLK_r)
    {
        // write random 'alive' cells to buffer
        std::vector<uint8_t>& writeBuffer = (frame % 2) ? bufferB : bufferA;
        for (int i = 0; i < writeBuffer.size(); i++) writeBuffer[i] = (random(0, 10) == 0) ? 1 : 0;
    }
}