// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_errDiffusion:
//
// Apply error diffusion algorithm to image I1.
//
// This procedure produces a black-and-white dithered version of I1.
// Each pixel is visited and if it + any error that has been diffused to it
// is greater than the threshold, the output pixel is white, otherwise it is black.
// The difference between this new value of the pixel from what it used to be
// (somewhere in between black and white) is diffused to the surrounding pixel
// intensities using different weighting systems.
//
// Use Floyd-Steinberg     weights if method=0.
// Use Jarvis-Judice-Ninke weights if method=1.
//
// Use raster scan (left-to-right) if serpentine=0.
// Use serpentine order (alternating left-to-right and right-to-left) if serpentine=1.
// Serpentine scan prevents errors from always being diffused in the same direction.
//
// A circular buffer is used to pad the edges of the image.
// Since a pixel + its error can exceed the 255 limit of uchar, shorts are used.
//
// Apply gamma correction to I1 prior to error diffusion.
// Output is saved in I2.
//

//this function copies a single row into the circlebuffer
void copyRowToCircBuffer(ChannelPtr<uchar> p1,short* circBuffer, int row, int w) {
	ChannelPtr<uchar> temp = p1;
	int wBuffer = w + 2;
	//copies a row of the original image to buffer, skipping padding numbers
	for (int i = 0; i < w; i++) {
		circBuffer[i + 1] = temp[i + (row * w)];
	}
}

//this function applies gamma correction onto Image I1 and stores it into I2
void gammaCorrection(ImagePtr I1, double gamma, ImagePtr I2){
	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);

	// init vars for width, height, and total number of pixels
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	// MaxGray as a double
	double mg = double(MaxGray);

	// init lookup table
	int i, lut[MXGRAY];
	for (i = 0; i < MXGRAY; ++i) lut[i] =  mg* pow((double)i/mg, (1 / gamma));

	// declarations for image channel pointers and datatype
	ChannelPtr<uchar> p1, p2;
	int type;

	// Note: IP_getChannel(I, ch, p1, type) gets pointer p1 of channel ch in image I.
	// The pixel datatype (e.g., uchar, short, ...) of that channel is returned in type.
	// It is ignored here since we assume that our input images consist exclusively of uchars.
	// IP_getChannel() returns 1 when channel ch exists, 0 otherwise.

	// visit all image channels and evaluate output image
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(I2, ch, p2, type);		// get output pointer for channel ch
		for (i = 0; i < total; i++) *p2++ = lut[*p1++];	// use lut[] to eval output
	}
}

//this function applies error diffusion using the jarvis jusice ninke method
void jarvis_judice_ninke_method(int w, int h, int type, ImagePtr I2, bool serpentine) {
	ChannelPtr<uchar> p1, p2;

	for (int ch = 0; IP_getChannel(I2, ch, p1, type); ch++) {

		int threshold = MXGRAY / 2;
		int total = w * h;
		int padding = 2;
		int wBuffer = w + 4;
		p2 = p1;
		uchar* image1 = new uchar[total];
		int circBufferSize = 3 * (w + 2 * padding);
		short* circBuffer = new short[circBufferSize];
		short* temp = circBuffer;
		int currentPointerRow = 0;

		//initilize all circBuffer elements to 0
		for (int i = 0; i < circBufferSize; i++)
		{
			circBuffer[i] = 0;
		}
		int y = 0;
		int x = 0;
		
		//initialize the circular buffer by adding rows 1 and 2
		copyRowToCircBuffer(p1, circBuffer, 0, w);
		circBuffer += wBuffer;
		currentPointerRow++;
		copyRowToCircBuffer(p1, circBuffer, 1, w);
		circBuffer += wBuffer;
		currentPointerRow++;


		//the circBufferPtrs are used to point to different elements in row 1,2,3
		short* in1 = temp + 2;				//circBufferPtr 1
		short* in2 = temp + 2 + (wBuffer);  //circBufferPtr 2
		short *in3 = temp + 2 + 2 * (wBuffer);	//circBUfferPtr3

		short* in1_temp = in1;
		short* in2_temp = in2;
		short* in3_temp = in3;

		//go through all rows
		for (int y = 0; y < h; y++) {

			if (currentPointerRow == 3) {
				circBuffer = temp;
				currentPointerRow = 0;
			}
			copyRowToCircBuffer(p1, circBuffer, y + 2, w);
			circBuffer += wBuffer;
			currentPointerRow++;

			if ((serpentine == 1) && (y % 2 == 1)) {
				in1_temp += (w - 1);
				in2_temp += (w - 1);
				in3_temp += (w - 1);
			}
				//go through each elements in the row
			for (int x = 0; x < w; x++) {

				//if serpentine and the current row is odd then apply serpentine method of err diffusion
				if ((serpentine == 1) && (y % 2 == 1)) {

					if (*in1_temp < threshold) {
						*p2 = 0;
					}
					else
					{
						*p2 = 255;
					}

					short e = *in1_temp - *p2;

					in1_temp[-1] += e * (7 / 48.);
					in1_temp[-2] += e * (5 / 48.);

					in2_temp[2] += e * (3 / 48.);
					in2_temp[1] += e * (5 / 48.);
					in2_temp[0] += e * (7 / 48.);
					in2_temp[-1] += e * (5 / 48.);
					in2_temp[-2] += e * (3 / 48.);

					in3_temp[2] += e * (1 / 48.);
					in3_temp[1] += e * (3 / 48.);
					in3_temp[0] += e * (5 / 48.);
					in3_temp[-1] += e * (3 / 48.);
					in3_temp[-2] += e * (1 / 48.);

					in1_temp--;
					in2_temp--;
					in3_temp--;
					p2++;

				}
				//if not serpentine then process lines rows left to right
				else {

					if (*in1_temp < threshold) {
						*p2 = 0;
					}
					else
					{
						*p2 = 255;
					}

					short e = *in1_temp - *p2;

					//main method to distribute weight to neighboring pixels
					in1_temp[1] += e * (7 / 48.);
					in1_temp[2] += e * (5 / 48.);

					in2_temp[-2] += e * (3 / 48.);
					in2_temp[-1] += e * (5 / 48.);
					in2_temp[0] += e * (7 / 48.);
					in2_temp[1] += e * (5 / 48.);
					in2_temp[2] += e * (3 / 48.);

					in3_temp[-2] += e * (1 / 48.);
					in3_temp[-1] += e * (3 / 48.);
					in3_temp[0] += e * (5 / 48.);
					in3_temp[1] += e * (3 / 48.);
					in3_temp[2] += e * (1 / 48.);

					in1_temp++;
					in2_temp++;
					in3_temp++;

					p2++;
				}

			}
			//decrement the pointer of each row back to the left side after serpentine reading of a odd row
			if ((serpentine == 1) && (y % 2 == 1)) {
				in1_temp -= (w - 1);
				in2_temp -= (w - 1);
				in3_temp -= (w - 1);
			}
				//after each row, shift the order from 1,2,3 / 3,1,2 / 2,3,1  for circular shifting
				in1_temp = in1;
				in2_temp = in2;
				in3_temp = in3;

				short* temp3 = in1;
				in1 = in2;
				in2 = in3;
				in3 = temp3;

			
		}
	}
}

//this function applies error diffusion using the floyd steinburg method method
void floyd_steinburg_method(int w, int h,int type, ImagePtr I2, bool serpentine) {
	
	ChannelPtr<uchar> p1, p2;
	
	for (int ch = 0; IP_getChannel(I2, ch, p1, type); ch++) {

		int threshold = MXGRAY / 2;
		int total = w * h;
		int padding = 1;
		int wBuffer = w + 2;
		p2 = p1;
		uchar* image1 = new uchar[total];
		int circBufferSize = 2 * (w + 2 * padding);
		short* circBuffer = new short[circBufferSize];
		short* temp = circBuffer;
		int currentPointerRow = 0;

		//initilize all circBuffer elements to 0
		for (int i = 0; i < circBufferSize; i++)
		{
			circBuffer[i] = 0;
		}
		int y = 0;
		int x = 0;

		copyRowToCircBuffer(p1, circBuffer, 0, w);
		circBuffer += wBuffer;
		currentPointerRow++;

		//the circBufferPtrs are used to point to different elements in row 1,2

		short* in1 = temp + 1;				//circBufferPtr 1
		short* in2 = temp + 1 + (wBuffer);  //circBufferPtr 1
		short* in1_temp = in1;
		short* in2_temp = in2;

		//go through all rows
		for (int y = 0; y < h; y++) {

			if (currentPointerRow == 2) {
				circBuffer = temp;
				currentPointerRow = 0;
			}
			copyRowToCircBuffer(p1, circBuffer, y + 1, w);
			circBuffer += wBuffer;
			currentPointerRow++;

			if ((serpentine == 1) && (y % 2 == 1)) {
				in1_temp += (w - 1);
				in2_temp += (w - 1);
			}
			//go through each elements in the row
			for (int x = 0; x < w; x++) {
				//if serpentine and the current row is odd then apply serpentine method of err diffusion

				if ((serpentine == 1) && (y%2 == 1)) {

					if (*in1_temp < threshold) {
						*p2 = 0;
					}
					else
					{
						*p2 = 255;
					}

					short e = *in1_temp - *p2;
					in1_temp[-1] += e * (7 / 16.);
					in2_temp[1] += e * (3 / 16.);
					in2_temp[0] += e * (5 / 16.);
					in2_temp[-1] += e * (1 / 16.);

					in1_temp--;
					in2_temp--;
					p2++;


				}
				//if not serpentine then process lines rows left to right
				else {

					if (*in1_temp < threshold) {
						*p2 = 0;
					}
					else
					{
						*p2 = 255;
					}
					//main method to distribute weight to neighboring pixels
					short e = *in1_temp - *p2;
					in1_temp[1] += e * (7 / 16.);
					in2_temp[-1] += e * (3 / 16.);
					in2_temp[0] += e * (5 / 16.);
					in2_temp[1] += e * (1 / 16.);

					in1_temp++;
					in2_temp++;
					p2++;
				}
			}
			//decrement the pointer of each row back to the left side after serpentine reading of a odd row
			if ((serpentine == 1) && (y % 2 == 1)) {
				in1_temp -= (w - 1);
				in2_temp -= (w - 1);
			}
			//after each row, shift the order from 1,2,3 / 3,1,2 / 2,3,1  for circular shifting
			in1_temp = in1;
			in2_temp = in2;

			short* temp2 = in1;
			in1 = in2;
			in2 = temp2;

		}
	}
}

void
HW_errDiffusion(ImagePtr I1, int method, bool serpentine, double gamma, ImagePtr I2)
{
	int w = I1->width();
	int h = I1->height();
	
	IP_copyImageHeader(I1, I2);
	
	ChannelPtr<uchar> p1, p2,temp, p2a;
	int type;

	gammaCorrection(I1, gamma, I2);

	if (method == 0) {

		floyd_steinburg_method(w, h,type,I2,serpentine);
	}

	else{
		jarvis_judice_ninke_method(w, h, type, I2,serpentine);

	}

	
}
