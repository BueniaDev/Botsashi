#ifndef SIMSASHI_BUILDER
#define SIMSASHI_BUILDER

#include <iostream>
#include <sstream>
#include <cstdint>
#include <cassert>
#include <SDL2/SDL.h>
using namespace std;

template<typename T>
bool inRange(T val, int low, int high)
{
    int value = int(val);
    return ((value >= low) && (value < high));
}

struct SimsashiPoint
{
    int xpos;
    int ypos;

    SimsashiPoint() : xpos(0), ypos(0)
    {

    }

    SimsashiPoint(int x, int y) : xpos(x), ypos(y)
    {

    }

    string to_str()
    {
	stringstream ss;
	ss << "(" << dec << int(x) << "," << dec << int(y) << ")";
	return ss.str();
    }
};

struct SimsashiRGB
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    SimsashiRGB() : red(0), green(0), blue(0)
    {

    }

    SimsashiRGB(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b)
    {

    }

    SimsashiRGB(uint32_t color)
    {
	red = uint8_t(color);
	green = uint8_t(color >> 8);
	blue = uint8_t(color >> 16);
    }

    uint32_t to_u32()
    {
	return ((blue << 16) | (green << 8) | red);
    }

    string to_str()
    {
	stringstream ss;
	ss << "(" << dec << int(red) << "," << dec << int(green) << "," << dec << int(blue) << ")";
	return ss.str();
    }
};

class SimsashiIO
{
    public:
	SimsashiIO(bool &cb) : stopped(cb), window_width(640), window_height(480)
	{

	}

	~SimsashiIO()
	{

	}

	bool init()
	{
	    if (SDL_Init(SDL_INIT_VIDEO) < 0)
	    {
		return sdl_error("SDL could not be initialized!");
	    }

	    window = SDL_CreateWindow("Simsashi", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);

	    if (window == NULL)
	    {
		return sdl_error("Window could not be created!");
	    }

	    render = SDL_CreateRenderer(window, -1, 0);

	    if (render == NULL)
	    {
		return sdl_error("Renderer could not be created!");
	    }

	    texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGB24,
SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

	    if (texture == NULL)
	    {
		sdl_error("Texture could not be created!");
		return false;
	    }

	    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	    SDL_RenderClear(render);

	    framebuffer.resize((window_width * window_height), SimsashiRGB(0, 0, 0));

	    return true;
	}

	void shutdown()
	{
	    if (texture != NULL)
	    {
		SDL_DestroyTexture(texture);
		texture = NULL;
	    }

	    if (render != NULL)
	    {
		SDL_DestroyRenderer(render);
		render = NULL;
	    }

	    if (window != NULL)
	    {
		SDL_DestroyWindow(window);
		window = NULL;
	    }

	    SDL_Quit();
	}

	void mainLoop(bool &quit)
	{
	    SDL_Event event;
	    while (SDL_PollEvent(&event))
	    {
		switch (event.type)
		{
		    case SDL_QUIT: quit = true; break;
		}
	    }

	    render_frame();
	}

	void setPenColor(uint32_t color)
	{

	}

	void setFillColor(uint32_t color)
	{

	}

	void setPixel(int xpos, int ypos)
	{

	}

	uint32_t getPixel(int xpos, int ypos)
	{
	    return 0;
	}

	void line(int x1, int y1, int x2, int y2)
	{
	    
	}

	void lineTo(int x, int y)
	{

	}

	void moveTo(int x, int y)
	{

	}

	void rectangle(int x1, int y1, int x2, int y2)
	{

	}

	void ellipse(int x1, int y1, int x2, int y2)
	{

	}

	void floodFill(int x, int y)
	{

	}

	void unfilledRectangle(int x1, int y1, int x2, int y2)
	{

	}

	void unfilledEllipse(int x1, int y1, int x2, int y2)
	{

	}

	void setDrawingMode(int mode)
	{

	}

	void setPenWidth(int width)
	{

	}

	void getWindowSize(uint16_t &width, uint16_t &height)
	{
	    width = window_width;
	    height = window_height;
	}

	void setWindowSize(uint16_t width, uint16_t height)
	{

	}

	void setFullScreen(bool is_enabled)
	{

	}

	void closeAllFiles()
	{
	    cout << "Closing all files..." << endl;
	}

	void openFile(string filename, uint16_t &ret, uint32_t &file_id)
	{
	    cout << "Loading file of " << filename << endl;
	    ret = 0;
	    file_id = 0;
	}

	void newFile(string filename, uint16_t &ret, uint32_t &file_id)
	{
	    cout << "Loading file of " << filename << endl;
	    ret = 0;
	    file_id = 0;
	}

	vector<uint8_t> readFile(uint32_t file_id, uint32_t num_bytes, uint16_t &ret)
	{
	    cout << "Reading from file..." << endl;
	    ret = 0;
	    vector<uint8_t> file_data;
	    file_data.resize(num_bytes, 0);
	    return file_data;
	}

	void writeFile(uint32_t file_id, vector<uint8_t> write_vec, uint32_t num_bytes, uint16_t &ret)
	{
	    cout << "Writing to file..." << endl;
	    ret = 0;
	}

	void seekFile(uint32_t file_id, uint32_t file_pos, uint16_t &ret)
	{
	    cout << "Seeking in file..." << endl;
	    ret = 0;
	}

	void deleteFile(string filename, uint16_t &ret)
	{
	    cout << "Deleting file of " << filename << endl;
	    ret = 0;
	}

	void setupWindow()
	{

	}

	void halt()
	{
	    stopped = true;
	}

	// TODO: Implement text rendering on graphics display
	void textOut(string str)
	{
	    cout << str;
	}

	void textOutCR(string str)
	{
	    cout << str << endl;
	}

	void loadWAV(string filename, uint8_t ref_num)
	{
	    cout << "Loading WAV file of " << filename << endl;
	}

	void playWAV(string filename, uint16_t &ret)
	{
	    cout << "Playing WAV file of " << filename << endl;
	    ret = 1;
	}

	void playSound(uint8_t ref_num, uint16_t &ret)
	{
	    ret = 1;
	}

	void take_screenshot()
	{
	    int width, height;
	    SDL_GetRendererOutputSize(render, &width, &height);
	    SDL_Surface *pix_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGB24);

	    if (pix_surface == NULL)
	    {
		sdl_error("Could not take screenshot!");
		return;
	    }

	    SDL_RenderReadPixels(render, NULL, pix_surface->format->format, pix_surface->pixels, pix_surface->pitch);

	    time_t currenttime = time(nullptr);
	    string filepath = "Simsashi_screenshot_";
	    filepath.append(to_string(currenttime));
	    filepath.append(".bmp");

	    SDL_SaveBMP(pix_surface, filepath.c_str());
	    SDL_FreeSurface(pix_surface);
	}

    private:
	bool &stopped;

	bool sdl_error(string msg)
	{
	    cout << msg << " SDL_Error: " << SDL_GetError() << endl;
	    return false;
	}

	void render_frame()
	{
	    assert(render && texture);
	    SDL_UpdateTexture(texture, NULL, framebuffer.data(), (window_width * sizeof(SimsashiRGB)));

	    SDL_RenderClear(render);
	    SDL_RenderCopy(render, texture, NULL, NULL);
	    SDL_RenderPresent(render);
	}

	SDL_Window *window = NULL;
	SDL_Renderer *render = NULL;
	SDL_Texture *texture = NULL;

	int window_width = 0;
	int window_height = 0;

	vector<SimsashiRGB> framebuffer;

	SimsashiRGB pen_color;
	SimsashiRGB fill_color;

	SimsashiPoint drawing_point;

	int pen_width = 0;
};


#endif // SIMSASHI_BUILDER