#include "CA.h"
#include "Texture.h"

bool CA::Initialize()
{
	buffer.resize(size.x * size.y);

	return true;
}

void CA::Step()
{
	// "draw" onto buffer
	Write<uint8_t>(buffer, size.x / 2, size.y, 1);

	// rule 30 = 00011110 (binary)
	//uint8_t rule[] = { 0, 0, 0, 1, 1, 1, 1, 0 };

	//uint8_t rule = 73; 
	//uint8_t rule = 225;
	//uint8_t rule = 99;
	uint8_t rule = 30; 

	// will give you headaches || flashes through all rule patterns up to 255
	//static uint8_t rule = 0;
	//rule++; 

	// update buffer
	for (int y = 0; y < size.y - 1; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			uint8_t i = 0;
			// read surrounding cells (x-1, x, x+2), a value between 0-7 can be created by shifting the bits
			i |= Read<uint8_t>(buffer, x - 1, y) << 2; // 000  
			i |= Read<uint8_t>(buffer, x, y) << 1; // 010  
			i |= Read<uint8_t>(buffer, x + 1, y) << 0; // 001  

			// elementary cellular automata rules
			uint8_t state = (rule & 1 << i) ? 1 : 0; // 7 - i prevents you from doin the reverse binary 
			Write<uint8_t>(buffer, x, y + 1, state); 
		}
	}

	// convert buffer data format to color buffer
	// go from beginning to end and then write to our color buffer
	std::transform(buffer.begin(), buffer.end(), color_buffer.begin(), [](uint8_t v) 
		{
			uint8_t color = v ? 255 : 0;
			return v ? (color << 24 | color << 16 | color << 8 | 0xff) : 0;
		});

	// copy color buffer to texture
	texture->Copy(color_buffer);
}