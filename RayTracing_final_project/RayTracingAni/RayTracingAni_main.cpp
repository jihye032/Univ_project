
#include "FL/Fl.H"
#include "FL/Fl_Double_Window.H"
#include "FL/Fl_RGB_Image.H"
#include <iostream>

#include "RayTracingAni.h"


#define RESOLUTION_W 720
#define RESOLUTION_H 480






// Window for Display
class ImageWin : public Fl_Double_Window
{
public:

	// FLTK RGB Image
	Fl_RGB_Image *image_;

	ImageWin(int w, int h, const char *s=0) : Fl_Double_Window(w, h, s)
	{
		image_ = 0;
	};


	
	virtual void draw()
	{
		Fl_Double_Window::draw();

	
		// Draw the Image unless it's null
		if ( image_ != 0 ) image_->draw(0, 0);

	}

	
};

// RGB Color Buffer
uchar3 *h_color_buffer;

// Window
ImageWin *win = 0;



// Animation Variables
float elapsed_time = 0.f;

void timeout_callback(void*) 
{
	// 흘러간 시간 계산.
	elapsed_time += 0.03f;

	// 0.03초 후에 timeout_callback이 다시 호출되도록 한다.
	// 결국 timeout_callback 함수를 0.03초마다 반복적으로 호출 된다.
	Fl::repeat_timeout(0.03f, timeout_callback);


	// 그림을 다시 만들고, 화면에 출력한다.
	if ( win != 0 )
	{
		UpdateImage(h_color_buffer, elapsed_time);
		win->image_->uncache();
		win->redraw();
	}
}



int main()
{
	
	h_color_buffer = new uchar3[RESOLUTION_W*RESOLUTION_H];

	AllocGPUBuffer(RESOLUTION_W, RESOLUTION_H);
	UpdateImage(h_color_buffer, elapsed_time);

	
	// Create FLTK Window for displaying the result.
	//ImageWin *win = new ImageWin(RESOLUTION_W, RESOLUTION_H);
	win = new ImageWin(RESOLUTION_W, RESOLUTION_H);
	// Create FLTK RGB Image with my color buffer.
	win->image_ = new Fl_RGB_Image((unsigned char*)h_color_buffer, RESOLUTION_W, RESOLUTION_H, 3);
	win->resizable(win);
	win->end();
	win->show();

	

	// 0.03초 후에 timeout_callback이 호출되도록 한다.
	Fl::add_timeout(0.03, timeout_callback);

	Fl::run();

	FreeGPUBuffer();
	

	return 0;
}