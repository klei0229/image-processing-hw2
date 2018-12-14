// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_sharpen:
//
// Sharpen image I1. Output is in I2.
//

//#include "hw2/HW_blur.cpp"

void blur_line1(ChannelPtr<uchar> input_Ptr, ChannelPtr<uchar> output_Ptr, int increment, int lineLength, int kernel_length) {

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

		avg = sum / kernel_length;
		*b = avg;
		b += increment;
	}

	int z = 0;
}


void HW_blurring(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
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

		for (int i = 0; i < h; i++) {

			blur_line1(p1 + (w*i), p2 + (w*i), 1, w, filterW);
		}


		for (int i = 0; i < w; i++) {

			blur_line1(p2 + i, p2 + i, w, h, filterH);
		}

	}
}


void
HW_sharpen(ImagePtr I1, int size, double factor, ImagePtr I2){
	int w = I1->width();
	int h = I1->height();
	int total = w * h;
	IP_copyImageHeader(I1, I2);

	ChannelPtr<uchar> p1, p2;
	int type;

	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		

		HW_blurring(I1, size, size, I2);
		IP_getChannel(I2, ch, p2, type);

		for (int i = 0; i < total; i++) {
			
			//sharpened pixel value is calculated by taking a blurred version of I1 and adding scale differenc
			int gray_level = ((int)*p1 - (int)*p2) * factor + (int)*p1;
			uchar gray_uc;

			//Clip the sharpened value
			if (gray_level < 0){
				gray_uc = 0;
			}
			else if (gray_level > 255) {
				gray_uc = 255;
			}
			else {
				gray_uc = gray_level;
			}

			//output the value into p2(output image)
			*p2 = gray_uc;
			p1 += 1;
			p2 += 1;
		}
	}
}
