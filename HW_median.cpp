#include <algorithm>
#include <vector>

using std::vector;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_median:
//
// Apply median filter of size sz x sz to I1.
// Clamp sz to 9.
// Output is in I2.
//
void
HW_median(ImagePtr I1, int sz, ImagePtr I2)
{
	if ((sz % 2) == 0) {
		sz += 1;
	}

	int w = I1->width();
	int h = I1->height();
	int total = w * h;
	IP_copyImageHeader(I1, I2);
	int padding = (sz - 1) / 2;

	ChannelPtr<uchar> p1, p2, temp;
	int type;

	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type);

		int widthBuffer = w + 2 * padding;
		int heightBuffer = h + 2 * padding;
		int bufferSize = widthBuffer * heightBuffer;
		int bufferSize2 = total;
		uchar* image1 = new uchar[total];
		uchar* buffer = new uchar[bufferSize];

		//fill the buffer with the padding
		//padding function
		//fill buffer with all 0's for debugging
		for (int i = 0; i < bufferSize; i++) {
			buffer[i] = 0;
		}

		//code snippet copies all of image 1 pixels into buffer
		int j = 0;
		int bufferIndex = padding + widthBuffer * padding;

		for (int i = 0; i < total; i++) {

			if (j == w) {
				bufferIndex += padding * 2;
				j = 0;
			}
			buffer[bufferIndex] = p1[i];
			j += 1;
			bufferIndex += 1;
		}

		//at this point buffer has image 1 pixels
		//pixel replicate top padding 
		//for each layer in the padding
		j = 0;

		uchar* buffer1D = new uchar[widthBuffer];

		for (int k = 0; k < padding; k++) {

			if (k == 0) {
				int pad1End = padding;
				int pad2End = padding + w;

				int p1_i = 0; //p1 index
				for (int h = 0; h < widthBuffer; h++) {

					if (h < pad1End) {
						buffer1D[h] = p1[p1_i];
						buffer[j + h] = p1[p1_i];


					}
					else if (h >= pad2End) {
						p1_i--;
						buffer1D[h] = p1[p1_i];
						buffer[j + h] = p1[p1_i];
						p1_i++;
					}
					else {
						buffer1D[h] = p1[p1_i];
						buffer[j + h] = p1[p1_i];
						p1_i++;
					}
				}
			}
			//for all other layers k > 1
			else {
				for (int h = 0; h < widthBuffer; h++) {
					buffer[j + h] = buffer1D[h];

				}
			}
			j += widthBuffer;
		}

		// pixel replicate bottom padding
		//for each layer in the padding
		j = (padding + h) * widthBuffer;
		for (int k = 0; k < padding; k++) {
			if (k == 0) {
				int pad1End = padding;
				int pad2End = padding + w;

				int p1_i = 0 + (h - 1) * w; //p1 index
				for (int h = 0; h < widthBuffer; h++) {
					if (h < pad1End) {
						buffer1D[h] = p1[p1_i];
						buffer[j + h] = p1[p1_i];
					}
					else if (h >= pad2End) {
						p1_i--;
						buffer1D[h] = p1[p1_i];
						buffer[j + h] = p1[p1_i];
						p1_i++;
					}
					else {
						buffer1D[h] = p1[p1_i];
						buffer[j + h] = p1[p1_i];
						p1_i++;
					}
				}
			}
			else {
				for (int h = 0; h < widthBuffer; h++) {
					buffer[j + h] = buffer1D[h];
				}
			}
			j += widthBuffer;
		}
		//pixel replicate for left padding
		j = 0;
		for (int k = 0; k < padding; k++) {

			if (k == 0) {
				int pad1End = (padding);
				int pad2End = (padding + w);

				int p1_i = 0;
				for (int h = 0; h < heightBuffer; h++) {
					if (h < pad1End) {
						buffer1D[h] = p1[p1_i];
						buffer[j + (h*widthBuffer)] = p1[p1_i];
					}
					else if (h >= pad2End) {
						p1_i -= w;
						buffer1D[h] = p1[p1_i];
						buffer[j + (h*widthBuffer)] = p1[p1_i];
						p1_i += w;
					}
					else {
						buffer1D[h] = p1[p1_i];
						buffer[j + (h*widthBuffer)] = p1[p1_i];
						p1_i += w;
					}
				}
				j++;
			}
			else {

				for (int h = 0; h < widthBuffer; h++) {
					buffer[j + h * widthBuffer] = buffer1D[h];

				}
				j++;
			}
		}
		//pixel replicate for right padding
		j = padding + w;
		for (int k = 0; k < padding; k++) {

			if (k == 0) {

				int pad1End = (padding);
				int pad2End = (padding + w);

				int p1_i = 0 + (w - 1);
				for (int h = 0; h < heightBuffer; h++) {
					if (h < pad1End) {
						buffer1D[h] = p1[p1_i];
						buffer[j + (h*widthBuffer)] = p1[p1_i];
					}
					else if (h >= pad2End) {
						p1_i -= w;
						buffer1D[h] = p1[p1_i];
						buffer[j + (h*widthBuffer)] = p1[p1_i];
						p1_i += w;
					}
					else {
						buffer1D[h] = p1[p1_i];
						buffer[j + (h*widthBuffer)] = p1[p1_i];
						p1_i += w;
					}
				}
				j++;
			}
			else {
				for (int h = 0; h < widthBuffer; h++) {
					buffer[j + h * widthBuffer] = buffer1D[h];
				}
				j++;
			}
		}
		temp = p1;
		for (int i = 0; i < total; i++)
		{
			image1[i] = *temp;
			temp++;
		}
		int cbuff_size = sz * sz;
		uchar* circ_buffer = new uchar[cbuff_size];
		int* position_buffer = new int[cbuff_size];

		int w1 = -1 * padding;
		int h1 = -1 * padding;

		int count = 0;
		int x = 0;

		for (int i = 0; i < cbuff_size; i++) {
			if (count == sz) {
				h1++;
				w1 = -1 * padding;
				count = 0;
			}
			x = (h1 * widthBuffer) + w1;
			w1++;
			count++;
			position_buffer[i] = x;
		}

		int startP1 = (padding * widthBuffer + padding);
		count = 0;

		temp = p2;
		//circular buffer 
		for (int i = 0; i < total; i++) {

			if (count == w) {
				startP1 += (2 * padding);
				count = 0;
			}

			for (int j = 0; j < cbuff_size; j++) {

				int temp = startP1 + position_buffer[j];
				
				circ_buffer[j] = buffer[temp];
			}

			startP1++;
			count++;

			//sort the circular buffer
			std::sort(circ_buffer, circ_buffer + sz * sz);

			//y is the center index or median index
			int y = cbuff_size / 2;
			int value = circ_buffer[y];

			//store the median into image p2
			*p2 = value;
			p2 += 1;
		}
		uchar* image2 = new uchar[total];

		for (int i = 0; i < total; i++)
		{
			image2[i] = *temp;
			temp++;
		}

	}
}
