// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur:
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
//#include <iostream>
//using namespace std;
//using System.Diagnostics;
//#define new DEBUG_NEW

#include <string>
//using namespace string;

//blurring either a single column or row
void blur_line(ChannelPtr<uchar> input_Ptr, ChannelPtr<uchar> output_Ptr, int increment, int lineLength, int kernel_length) {

	ChannelPtr<uchar> a = input_Ptr;
	ChannelPtr<uchar> b = output_Ptr;
	int padding = (kernel_length - 1) / 2;

	//instatiate buffer
	int bufferSize = lineLength + 2 * padding;
	uchar* buffer = new uchar[bufferSize];

	for (int i = 0; i < bufferSize; i++) {

		//pixel replication
		if (i < padding) {
			buffer[i] = *a;
		}
		//pixel replication
		else if (i >= (padding + lineLength))
		{
			a -= increment;
			buffer[i] = *a;
			a += increment;
		}

		else {
			buffer[i] = *a;			
			a += increment;
		}
	}

	//buffer finished
	int sum = 0;
	int avg = 0;
	int counter = 0;
	for (int j = 0; j < lineLength; j++) {

		//start the calculation
		if (j == 0) {
			for (int k = 0; k < kernel_length; k++) {
				sum += buffer[k];
			}
		}
		else {
			sum -= buffer[j - 1];
			sum += buffer[j + (padding * 2)];
		}

		//calcute the average of the neighborhood and store into output image
		avg = sum / kernel_length;
		*b = avg;
		b += increment;
	}

	int z = 0;
}

void
HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
	//if the kernel size is even, increment to the next odd kernel 
	if (filterW % 2 == 0) {
		filterW += 1;
	}

	if (filterH % 2 == 0) {
		filterH += 1;
	}

	int w = I1->width();
	int h = I1->height();
	int total = w * h;
	int i;

	IP_copyImageHeader(I1, I2);
	
	ChannelPtr<uchar> p1, p2, p3, p4, p5;
	int type;

	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {

		IP_getChannel(I2, ch, p2, type);
		// apply blurring to the rows and store the value into p2
		for (int i = 0; i < h; i++) {
			blur_line(p1+(w*i), p2+(w*i), 1, w, filterW);
		}
		//apply blurrring to the columns of p2 after it has been horizontally blurred
		for (int i = 0; i < w; i++) {
			blur_line(p2 + i, p2 + i, w, h, filterH);
		}

	}
}
