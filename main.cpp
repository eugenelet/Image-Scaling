#include "bitmapRW.h"

using namespace std;
unsigned char *resizeBilinear(unsigned char *input_img, 
	int w_in, int h_in, int w_out, int h_out, int bytesPerPixel){
	unsigned char *resizedImage = new unsigned char[w_out*h_out*bytesPerPixel];
	int A, B, C, D, x, y, index;
	float x_ratio = ((float) (w_in - 1))/w_out;
	float y_ratio = ((float) (h_in - 1))/h_out;
	float x_diff, y_diff;
	cout << "Width(Input): " << w_in << "\tHeight(Input): " << h_in << endl;
	cout << "Width(Output): " << w_out << "\tHeight(Output): " << h_out << endl;
	cout << "Bytes Per Pixel: " << bytesPerPixel << endl;

	for(int row = 0; row < h_out; row++)
		for(int col = 0; col < w_out; col++){
			x = (int) (x_ratio * col);
			y = (int) (y_ratio * row);
			x_diff = (x_ratio * col) - x;
			y_diff = (y_ratio * row) - y;
			for(int count = 0; count < bytesPerPixel; count++){
				index  = y*w_in*bytesPerPixel + x*bytesPerPixel + count;
				A = input_img[index];
				B = input_img[index + bytesPerPixel];
				C = input_img[index + w_in*bytesPerPixel];
				D = input_img[index + w_in*bytesPerPixel + bytesPerPixel];
				resizedImage[row*w_out*bytesPerPixel + col*bytesPerPixel + count] = 
					(unsigned char) (A*(1-x_diff)*(1-y_diff) + B*(x_diff)*(1-y_diff) +
                   	C*(y_diff)*(1-x_diff)   + D*(x_diff*y_diff));
			}
		}

	return resizedImage;
}

int main(int argc, char** argv){

    if ( (argc < 4) || (strcmp(argv[1],"-h") == 0) ) { 
          cerr << "usage: ./[executable file name] [input file name] [output file name] [scale factor]" << endl;
          cerr << "scale factor" << endl;
          cerr << "------------" << endl;
          cerr << "1 ~ N: Upscale N" << endl;
          cerr << "0 ~ 1: Downscale 1/N" << endl;
          exit(1);
    } 

    char* input_file 	= argv[1];
    char* output_file 	= argv[2];
    float scaleFactor 	= atof(argv[3]);

	BITMAPINFOHEADER *bitmapInfoHeader = new BITMAPINFOHEADER;
	BITMAPFILEHEADER *bitmapFileHeader = new BITMAPFILEHEADER;
	unsigned char *bitmapData;
	bitmapData = LoadBitmapFile(input_file, bitmapInfoHeader, bitmapFileHeader);
	if (scaleFactor < 1)
		cout << "Down Scale by " << 1.0 /scaleFactor << endl;
	else
		cout << "Up Scale by " << scaleFactor << endl;
	int w_out = ((float) bitmapInfoHeader->biWidth) * scaleFactor;
	int h_out = ((float) bitmapInfoHeader->biHeight) * scaleFactor;
	int bytesPerPixel = (bitmapInfoHeader->biBitCount) / 8;
	bitmapData = resizeBilinear(bitmapData, bitmapInfoHeader->biWidth, bitmapInfoHeader->biHeight,
		 w_out, h_out, bytesPerPixel);

	bitmapInfoHeader->biWidth = w_out;
	bitmapInfoHeader->biHeight = h_out;
	if(bytesPerPixel!=4 && ((bytesPerPixel*bitmapInfoHeader->biWidth)%4 != 0)){
		// int temp = (bytesPerPixel*w_out)/4;
		bitmapInfoHeader->biSizeImage = w_out*h_out*bytesPerPixel 
		+ (4 - (bytesPerPixel*w_out)%4)*h_out;
	}
	else
		bitmapInfoHeader->biSizeImage = w_out*h_out*bytesPerPixel;
	bitmapFileHeader->bfSize = bitmapInfoHeader->biSizeImage + bitmapFileHeader->bfOffBits;

	

	WriteBitmapFile(output_file, bitmapInfoHeader, bitmapFileHeader, bitmapData);

}

