// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_convolve:
//
// Convolve I1 with filter kernel in Ikernel.
// Output is in I2.
//

uchar clip1(float value,int t1,int t2) {
	if (value < t1 && value < MXGRAY) {
		value = t1;
	}
	else if(value < t2 && value < MXGRAY) {
		value = value;
	}
	
	else if (value < MXGRAY) {
		value = t2;
	}

	return uchar(value);

}
void
HW_convolve(ImagePtr I1, ImagePtr Ikernel, ImagePtr I2)
{
	int w = I1->width();
	int h = I1->height();
	int total = w * h;
	IP_copyImageHeader(I1, I2);
	ChannelPtr<uchar> p1, p2, temp;
	ChannelPtr<float> k1;
	int type;

	int widthKernel = Ikernel->width();
	int heightKernel = Ikernel->height();
	int totalKernel = widthKernel * heightKernel;

	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		
		IP_getChannel(Ikernel, ch, k1, type);
		IP_getChannel(I2, ch, p2, type);
	}

	float* kernel_array = new float[totalKernel];
	
	for (int i = 0; i < totalKernel; i++) {
		
		kernel_array[i] = *k1;
		k1++;
	}
		
	int padding = (widthKernel - 1) / 2;
	//code snippet copies all of image 1 pixels into buffer
	int widthBuffer = w + 2 * padding;
	int heightBuffer = h + 2 * padding;

	int bufferSize = widthBuffer * heightBuffer;
	int bufferSize2 = total;
	int j = 0;
	int bufferIndex = padding + widthBuffer * padding;

	uchar* buffer = new uchar[bufferSize];

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
			//print("here");
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
	//padding done

	int cbuff_size = totalKernel;
	uchar* circ_buffer = new uchar[cbuff_size];
	int* position_buffer = new int[cbuff_size];


	int w1 = -1 * padding;
	int h1 = -1 * padding;


	int count = 0;
	int x = 0;

	for (int i = 0; i < cbuff_size; i++) {

		if (count == widthKernel) {
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

		float sum = 0;
		
		//take the weighed sum of the pixels in the neighborhood and store to the current pixel
		for (int i = 0; i < cbuff_size; i++) {
			sum += kernel_array[i] * circ_buffer[i];
	
		}
		
		uchar clippedSum = clip1(ROUND(sum), 0, MXGRAY);
		*p2 = clippedSum;
		p2 += 1;
		sum = 0;
	}
	

}

