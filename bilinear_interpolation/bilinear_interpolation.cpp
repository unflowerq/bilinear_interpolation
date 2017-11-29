#include <iostream>
#include <fstream>

using namespace std;

void MemFree2D(unsigned char **Mem, int nHeight)
{
	for (int n = 0; n < nHeight; n++)
	{
		delete[] Mem[n];
	}
	delete[] Mem;
}

unsigned char** MemAlloc2D(int nHeight, int nWidth, unsigned char nInitVal)
{
	unsigned char** rtn = new unsigned char*[nHeight];
	for (int n = 0; n < nHeight; n++)
	{
		rtn[n] = new unsigned char[nWidth];
		memset(rtn[n], nInitVal, sizeof(unsigned char) * nWidth);
	}
	return rtn;
}

bool isInsideBoundary(int nHeight, int nWidth, double h, double w)
{
	if (h >= 0 && w >= 0 && h < nHeight && w < nWidth)
	{
		return true;
	}
	else return false;
}

unsigned char BilinearInterpolation(unsigned char** In, int nHeight_Ori, int nWidth_Ori, double h_Cvt, double w_Cvt)
{
	unsigned char r1, r2, r3, r4;
	unsigned char return_value;
	int h1, w1, h2, w2, h3, w3, h4, w4;

	h1 = floor(h_Cvt); w1 = floor(w_Cvt);
	h2 = h1; w2 = w1 + 1;
	h3 = h1 + 1; w3 = w1;
	h4 = h1 + 1; w4 = w1 + 1;

	if (h1 == nHeight_Ori - 1 && w1 == nWidth_Ori - 1)
		return 0;
	else if (h1 == nHeight_Ori - 1 && w1 != nWidth_Ori - 1)
		return 0;
	else if (h1 != nHeight_Ori - 1 && w1 == nWidth_Ori - 1)
		return 0;
	else
	{
		r1 = In[h1][w1];
		r2 = In[h2][w2];
		r3 = In[h3][w3];
		r4 = In[h4][w4];

		return_value = (h3 - h_Cvt)*((w2 - w_Cvt)*r1 + (w_Cvt - w1)*r2) + (h_Cvt - h1)*((w2 - w_Cvt)*r3 + (w_Cvt - w1)*r4);
		return return_value;
	}
}


int main()
{
	int width = 256;
	int height = 256;

	unsigned char **output_img512;
	unsigned char **output_img436;

	output_img512 = MemAlloc2D(512, 512, 0);
	output_img436 = MemAlloc2D(436, 436, 0);

	FILE *inFile;
	fopen_s(&inFile, "lena256.raw", "rb");

	if (inFile == NULL)
	{
		cout << "input image error";
		return 0;
	}

	unsigned char **input_img;

	input_img = MemAlloc2D(width, height, 0);

	for (int h = 0; h < height; h++)
	{
		fread(input_img[h], sizeof(unsigned char), width, inFile);
	}

	FILE *outFile512;
	fopen_s(&outFile512, "lena_cvt512.raw", "wb");

	FILE *outFile436;
	fopen_s(&outFile436, "lena_cvt436.raw", "wb");

	//////////////512크기의 이미지 바이리니어
	for (int h = 0; h < 512; h++)
	{
		for (int w = 0; w < 512; w++)
		{
			double h_Ori = h / 2;
			double w_Ori = w / 2;
			if (isInsideBoundary(512, 512, 2, 2))
			{
				output_img512[h][w] = BilinearInterpolation(input_img, height, width, h_Ori, w_Ori);
			}
		}
	}

	//////////////512파일에다 쓰기
	for (int h = 0; h < 512; h++)
	{
		fwrite(output_img512[h], sizeof(unsigned char), 512, outFile512);
	}

	//////////////436크기의 이미지 바이리니어
	for (int h = 0; h < 436; h++)
	{
		for (int w = 0; w < 436; w++)
		{
			double h_Ori = h / 1.703125;
			double w_Ori = w / 1.703125;
			if (isInsideBoundary(436, 436, 1.703125, 1.703125))
			{
				output_img436[h][w] = BilinearInterpolation(input_img, height, width, h_Ori, w_Ori);
			}
		}
	}

	//////////////436파일에다 쓰기
	for (int h = 0; h < 436; h++)
	{
		fwrite(output_img436[h], sizeof(unsigned char), 436, outFile436);
	}

	MemFree2D(input_img, 256);
	MemFree2D(output_img512, 512);
	MemFree2D(output_img436, 436);
	fclose(inFile);
	fclose(outFile512);
	fclose(outFile436);

	return 0;
}