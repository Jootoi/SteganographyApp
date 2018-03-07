/*
Copyright © 2018 Joonas Toimela <Toimela.Joonas.J@student.uta.fi>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/


#include"stdafx.h"
#include<fstream>
#include<cmath>
#include<random>
#include<unordered_set>
#include<set>
#include<ctime>
#include"steganography.hpp"




//Returns pixel data in 3 byte RGB format.Indexing starts from bottom left.
unsigned char* Steg::Bitmap::getPixel(unsigned int index) {
	unsigned char* RGB = new unsigned char[3];

	if (index <= this->DIB->pixels_w * std::abs(this->DIB->pixels_h)&& this->DIB->bits_per_pixel == 24) {
		unsigned int actual_start = 0;
		actual_start = index * 24;
		unsigned int padding = ((this->DIB->pixels_w * 24) % 32 == 0) ? 0:32 - (this->DIB->pixels_w*24)%32;
		actual_start += padding * (int)(index / this->DIB->pixels_w);

		RGB[0] = this->data[(actual_start + 16)/8];
		RGB[1] = this->data[(actual_start + 8)/8];
		RGB[2] = this->data[actual_start/8];
	}
	return RGB;
}

//Returns pixel data in 3 byte RBG format. Indexing starts from bottom left (and 0, as always).
unsigned char* Steg::Bitmap::getPixel(unsigned int height, unsigned int width) {
	unsigned char* RGB = new unsigned char[3];
	if (height < std::abs(this->DIB->pixels_h) && width < this->DIB->pixels_w) {
		unsigned int index = height * this->DIB->pixels_w + width;
		RGB = getPixel(index);
	}
	return RGB;
}

//Sets pixel RBG data. Excact opposite of get: setPixel(getPixel(n),n) would not change anything.
void Steg::Bitmap::setPixel(unsigned char * rgb, unsigned int index) {
	if (index <= this->DIB->pixels_w * std::abs(this->DIB->pixels_h) && this->DIB->bits_per_pixel == 24) {
		unsigned int actual_start = 0;
		actual_start = index * 24;
		unsigned int padding = ((this->DIB->pixels_w * 24) % 32 == 0) ? 0 : 32 - (this->DIB->pixels_w * 24) % 32;
		actual_start += padding * (int)(index / this->DIB->pixels_w);

		this->data[(actual_start + 16) / 8] = rgb[0];
		this->data[(actual_start + 8) / 8] = rgb[1];
		this->data[actual_start / 8] = rgb[2];
	}
}


//Saves bitmap file with BITMAPINFOHEADER, no compression and default color palette.
int Steg::Bitmap::save(std::string name) {
	std::ofstream file(name, std::fstream::out | std::fstream::binary);
	int zero = 0;
	char16_t one = 1;
	if (file.is_open()) {
		file.write(reinterpret_cast<char *>(this->bmHeader), 14);
		file.write(reinterpret_cast<char *>(this->DIB), 40);
		file.write(reinterpret_cast<char*> (this->data), (this->bmHeader->file_size-this->bmHeader->data_offset));
		
		file.close();
		return 0;
	}
	file.close();
	return 1;
}


//Randomly embedds bits to least significant bits of the image. Unsigned int sets limits to maximum image size (about 26kx26k pixels) and to maximum payload size (above 500GB).
bool Steg::Bitmap::randomEmbedd(unsigned int key, unsigned char * data, unsigned int length, int number_of_color_planes, int lsb) {
	unsigned int max_size = std::abs(this->DIB->pixels_h)*this->DIB->pixels_w*number_of_color_planes*lsb;
	unsigned char mask = 255 << lsb;
	if (length*8 <= max_size) {
		unsigned int * randArr = Steg::Shuffle(key, max_size/lsb, length*8);
		unsigned char least_significant = 255>>(8-lsb);
		for (unsigned int i = 0;i < length;++i) {
			char byte = data[i];
			for (int j = 0;j < 8 / lsb;++j) {
				unsigned char tmp = byte >> j*lsb;
				tmp = tmp & least_significant;
				unsigned char* pixel = this->getPixel((unsigned int)(randArr[i*(8/lsb) + j] / number_of_color_planes));
				unsigned char value = pixel[randArr[i*(8 / lsb) + j] % number_of_color_planes];
				//first 8-lsb bits stay same and lsb last bits become 1
				value = value | !mask;
				//then lsb last bits will be changed to 0
				value = value & mask;
				//finally "OR" will write required data to last bits
				value = value | tmp;
				pixel[randArr[i*(8 / lsb) + j] % number_of_color_planes] = value;
				this->setPixel(pixel, (unsigned int)(randArr[i*(8 / lsb) + j] / number_of_color_planes));
				delete[] pixel;
			}
		}
		delete[] randArr;
	}
	else {
		return false;
		std::cout << "\nCannot hide that much data.";
	}
	return true;
}








Steg::Bitmap* Steg::readBitmap(std::string name) {
	Bitmap* bm = new Bitmap;
    std::ifstream data (name, std::fstream::in|std::fstream::binary);
	if (data.is_open()) {
		Steg::BitmapHeader* h = new Steg::BitmapHeader;
		data.read(reinterpret_cast<char*>(h), 14);
		if (h->header_field == 19778) {
			Steg::BITMAPINFOHEADER* dib = new Steg::BITMAPINFOHEADER;
			data.read(reinterpret_cast<char*>(dib), 40);
			bm->bmHeader = h;
			bm->DIB = dib;
			if (bm->DIB->bits_per_pixel == 24 && bm->DIB->compression == 0) {
				//read pixel data
				int data_size = bm->bmHeader->file_size - bm->bmHeader->data_offset;

				char *image_buffer = new char[data_size];
				data.seekg(bm->bmHeader->data_offset);
				data.read(image_buffer, data_size);
				bm->data = reinterpret_cast<unsigned char*> (image_buffer);
			}
			else {
				std::cout << "File was not valid bitmap. Bitmap needs to have 24 bits per pixel and no compression";
			}
		}
		else {
			std::cout << "File was not valid bitmap";
			data.close();
			return NULL;
		}

	}
	else {
		std::cout << "Failed to open file.";
		data.close();
		return NULL;
	}
	data.close();
	return bm;
}

//Knuth-Fisher-Yates shuffle to get random array of non-repeating numbers
unsigned int* Steg::Shuffle(unsigned int key, unsigned int length, unsigned int size) {
	if (length / size < 50) {
		return Steg::KNU_Shuffle(key, length);
	}
	else {
		return Steg::RandomArray(key, length, size);
	}
}

unsigned char* Steg::Bitmap::decode(unsigned int key, unsigned int length,int number_of_color_planes, int lsb) {
	unsigned int max_size = std::abs(this->DIB->pixels_h)*this->DIB->pixels_w*number_of_color_planes*lsb;
	unsigned char* data_array = new unsigned char[length];
	if (length*8 <= max_size) {
		unsigned int * randArr = Steg::Shuffle(key, max_size / lsb,length*8);
		unsigned char least_significant = 255 >> (8 - lsb);
		for (unsigned int i = 0;i < length;++i) {
			unsigned char byte = 0;
			for (int j = 0;j < 8 / lsb;++j) {
				unsigned char* pixel = this->getPixel((unsigned int)(randArr[i*(8 / lsb) + j] / number_of_color_planes));
				unsigned char value = pixel[randArr[i*(8 / lsb) + j] % number_of_color_planes];
				value = value & least_significant;
				value = value << j*lsb;
				byte = byte | value;
				delete[] pixel;
			}
			data_array[i] = byte;
		}
		delete[] randArr;
	}
	return data_array;
}

unsigned int* Steg::KNU_Shuffle(unsigned int key, unsigned int length) {
	std::mersenne_twister_engine<std::uint_fast32_t, 32, 624, 397, 31,
		0x9908b0df, 11,
		0xffffffff, 7,
		0x9d2c5680, 15,
		0xefc60000, 18, 1812433253> gen(key);
	unsigned int* randomArray = new unsigned int[length];
	for (unsigned int i = 0;i < length;i++) {
		randomArray[i] = i;
	}

	for (;length > 0;--length) {
		unsigned int r = (gen() % length);
		unsigned int tmp = randomArray[length - 1];
		randomArray[length - 1] = randomArray[r];
		randomArray[r] = tmp;
		//std::swap(randomArray[length - 1], randomArray[r]);
	}
	return randomArray;
}

//This ended up beign even less efficient than I tought.
unsigned int* Steg::RandomArray(unsigned int key, unsigned int length, unsigned int size) {
	std::mersenne_twister_engine<std::uint_fast32_t, 32, 624, 397, 31,
		0x9908b0df, 11,
		0xffffffff, 7,
		0x9d2c5680, 15,
		0xefc60000, 18, 1812433253> gen(key);
	std::unordered_set<unsigned int> randomSet;
	randomSet.reserve(size);
	unsigned int* rand = new unsigned int[size];
	while (randomSet.size()<size) {
		randomSet.insert(gen() % length);
	}

	int i = 0;
	for (unsigned int val : randomSet) {
		rand[i] = val;
		i++;
	}
	return rand;
}

bool Steg::Bitmap::analyze() {
	std::pair<unsigned int, unsigned int>* firstMeasure = Steg::countColors(this);
	std::srand(std::time(0));
	unsigned int key = std::rand() % std::numeric_limits<unsigned int>::max();
	unsigned int maxCapacity = std::abs(this->DIB->pixels_h)*this->DIB->pixels_w * 3;
	unsigned int testLength = maxCapacity / 32;
	this->randomEmbedd(key, this->data, testLength);
	std::pair<unsigned int, unsigned int>* secondMeasure = Steg::countColors(this);
	std::cout << "\nUnique colors: " << firstMeasure->first << " Close colors: " << firstMeasure->second;
	std::cout << "\nUnique colors after modification: " << secondMeasure->first << " Close colors: " << secondMeasure->second;
	double firstRatio = firstMeasure->second /(double) (firstMeasure->first / 2);
	double secondRatio = secondMeasure->second /(double) (secondMeasure->first / 2);
	std::cout << "\nRatios to compare: " << firstRatio << " to " << secondRatio;
	if (firstRatio*1.1 < secondRatio) {
		return false;
	}
	else return true;
}



std::pair<unsigned int, unsigned int>* Steg::countColors(Steg::Bitmap* bm) {
	std::set<unsigned char*, Steg::cmp> s;
	unsigned int pixelCount = bm->DIB->pixels_w*std::abs(bm->DIB->pixels_h);
	unsigned char bitmask = 1;
	if (pixelCount < 1000000) {
		for (unsigned int i = 0;i < pixelCount;++i) {
			unsigned char* pixel = bm->getPixel(i);
			bool succes = s.insert(pixel).second;
			if (!succes) {
				delete[] pixel;
			}
		}
	}
	else {
		unsigned int * sampleArray = Shuffle(42, pixelCount, 1000000);
		for (unsigned int i = 0;i < 1000000;++i) {
			unsigned char* pixel = bm->getPixel(sampleArray[i]);
			bool succes = s.insert(pixel).second;
			if (!succes) {
				delete[] pixel;
			}
		}
		delete[] sampleArray;
	}
	unsigned int totalColors = s.size();
	//Filter for red
	for (std::set<unsigned char*>::iterator it = s.begin();it != s.end();) {
		unsigned char* first = *it;
		std::set<unsigned char*>::iterator tmp_it = it;
		++it;
		if (it != s.end()) {
			unsigned char* second = *it;
			if (!((first[0] == second[0]) || ((first[0] ^ bitmask) == second[0]))) {
				s.erase(tmp_it);
				delete[] first;
			}
		}
	}
	//filter for green
	for (std::set<unsigned char*>::iterator it = s.begin();it != s.end();) {
		unsigned char* first = *it;
		std::set<unsigned char*>::iterator tmp_it = it;
		++it;
		if (it != s.end()) {
			unsigned char* second = *it;
			if (!((first[1] == second[1]) || ((first[1] ^ bitmask) == second[1]))) {
				s.erase(tmp_it);
				delete[] first;
			}
		}
	}
	//filter for blue
	for (std::set<unsigned char*>::iterator it = s.begin();it != s.end();) {
		unsigned char* first = *it;
		std::set<unsigned char*>::iterator tmp_it = it;
		++it;
		if (it != s.end()) {
			unsigned char* second = *it;
			if (!((first[2] == second[2]) || ((first[2] ^ bitmask) == second[2]))) {
				s.erase(tmp_it);
				delete[] first;
			}
		}
	}

	std::pair<unsigned int, unsigned int>* p = new std::pair<unsigned int, unsigned int>;
	p->first = totalColors;
	p->second = s.size();
	for (unsigned char* p : s) {
		delete[] p;
	}
	return p;
}

bool Steg::cmp::operator()(unsigned char* a, unsigned char* b) {
	if (a[0] < b[0]) {
		return true;
	}
	else if (a[0] > b[0]) {
		return false;
	}
	else {
		if (a[1] < b[1]) {
			return true;
		}
		else if (a[1] > b[1]) {
			return false;
		}
		else {
			if (a[2] < b[2]) {
				return true;
			}
			else if (a[2] > b[2]) {
				return false;
			}
			else return false;
		}
	}
}

