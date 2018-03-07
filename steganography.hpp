/*
Copyright © 2018 Joonas Toimela <Toimela.Joonas.J@student.uta.fi>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/



#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H
#include"stdafx.h"
namespace Steg {
	#pragma pack(1)
struct BitmapHeader {
	char16_t header_field;
	int file_size;
	char16_t reserved1;
	char16_t reserved2;
	int data_offset;
};
struct BITMAPINFOHEADER {
	int DIB_size;
	int pixels_w;
	int pixels_h;
	char16_t color_planes;
	char16_t bits_per_pixel;
	int compression;
	int image_size;
	int resolution_h;
	int resolution_v;
	int colors;
	int important_colors;
};
	#pragma pack()
class Bitmap {
public:
	BitmapHeader* bmHeader;
	BITMAPINFOHEADER * DIB;
	//Pixel data
    unsigned char* data;

	//Get pixel data in rbg format from index specified as parameter. Indexing starts from bottom left.
	unsigned char* getPixel(unsigned int);

	//Get pixel data in rgb format from height/width specified as parameter. Indexing starts from bottom left.
	unsigned char* getPixel(unsigned int, unsigned int);

	//Sets pixel in index specified in second argument to have rbg values specified in first argument. Indexing starts from bottom left.
	void setPixel(unsigned char *, unsigned int);

	//Saves the bitmap with name specified in argument, do not forget format specifier (.bmp).
	int save(std::string);

	//Embedds data (second argument) to the bitmap to random pixels (random generator key is first argument). Third arguments tells the length of data to be hidden in bytes. 
	//4th argument allows hiding to only some of the color planes (red,green,blue) and 5th arguments allows using more than only the least significant bit from each byte (needs to be 1,2,4 or 8)
	bool randomEmbedd(unsigned int ,unsigned char*,unsigned int, int = 3, int = 1);

	//Decodes data hidden by randomEmbedd function. Random generator seed is first argument, second argument is length of message to decode and 3rd and 4th arguments are same as RandomEmbedds 4th and 5th.
	unsigned char* decode(unsigned int, unsigned int, int = 3, int = 1);
	 

	bool analyze();

	~Bitmap() {
		delete bmHeader;
		delete DIB;
		delete[] data;
	}
};
//Reads bitmap file to Steg::Bitmap class, argument specifies name of the bitmap file.
Bitmap* readBitmap(std::string);

//Creates random non-repeating array of values, length argument specifies the maximum value and size determines desired size of the array. Key is used to initilize the random generator.
//Actual size of the returned array is determined by which function is selected in the end.
unsigned int* Shuffle(unsigned int key, unsigned int length, unsigned int size);






//Everyting below this comment is in the header just for benchmarking purposes

//Knuth-Fisher-Yates shuffle to get random array of non-repeating numbers. Requires shuffling all the possibilities even when only subset is needed. Very efficient for all sizes.
unsigned int* KNU_Shuffle(unsigned int key, unsigned int length);

//Assigns non-repeating pseudo-random numbers to array. Returns array with size = "size" and numbers from [0,length[. Becomes less efficient as "size" increases and as "size" aproaches "length".
unsigned int* RandomArray(unsigned int key, unsigned int length, unsigned int size);

std::pair<unsigned int, unsigned int>* countColors(Steg::Bitmap* bm);



struct cmp {
	bool operator()(unsigned char*, unsigned char*);
};
} //end namespace
#endif // STEG_H
