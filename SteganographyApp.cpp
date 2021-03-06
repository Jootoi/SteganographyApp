/*
Copyright © 2018 Joonas Toimela <Toimela.Joonas.J@student.uta.fi>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/


#include"stdafx.h"
#include"steganography.hpp"
#include<fstream>
#include<ctime>
#include<limits>


void encodeFunc();
void decodeFunc();
void analyseFunc();
int main(int argc, char* argv[])
{
	std::cout << "Simple Steganography App By\n"
		<< "        _____                    ________         __	\n"
		<< "       /     |                  /        |       /  |	\n"
		<< "       $$$$$ |  ______    ______$$$$$$$$/______  $$/	\n"
		<< "          $$ | /      \\  /      \\  $$ | /      \\ /  |	\n"
		<< "     __   $$ |/$$$$$$  |/$$$$$$  | $$ |/$$$$$$  |$$ |	\n"
		<< "    /  |  $$ |$$ |  $$ |$$ |  $$ | $$ |$$ |  $$ |$$ |	\n"
		<< "    $$ \\__$$ |$$ \\__$$ |$$ \\__$$ | $$ |$$ \\__$$ |$$ |	\n"
		<< "    $$    $$/ $$    $$/ $$    $$/  $$ |$$    $$/ $$ |	\n"
		<< "     $$$$$$/   $$$$$$/   $$$$$$/   $$/  $$$$$$/  $$/	\n\n";

	std::string functionality;
	std::cout << "What do you want to do? (encode/decode/analyse): ";
	std::cin >> functionality;

	if (functionality == "encode" || functionality == "e"||functionality=="ENCODE"||functionality=="E") {
		encodeFunc();
	}
	else if (functionality == "decode" || functionality == "d" || functionality == "DECODE" || functionality == "D") {
		decodeFunc();
	}
	else if(functionality == "analyse" || functionality == "a" || functionality == "ANALYSE" || functionality == "A") {
		analyseFunc();
	}
	else {
		std::cout << "\nFunctionality not supported (yet?).";
		std::cout << "\nPress \"enter\" to quit";
		std::cin.ignore(); std::cin.get();
		exit(1);

	}
}

void encodeFunc() {
	std::string cover_name;
	std::string stego_name;
	std::string data_name;
	unsigned int key = 0;
	std::cout << "\nName of/path to cover image: ";
	std::cin >> cover_name;
	Steg::Bitmap* bm = Steg::readBitmap(cover_name);
	if (bm != NULL) {
		std::cout << "\nName of/path to file to be hidden: ";
		std::cin >> data_name;
		std::ifstream data(data_name, std::ios_base::binary | std::ios_base::ate);
		if (data.is_open()) {
			std::cout << "\nKey required for decoding, if 0 is given a pseudo-random key is generated and encoded to the image in pre-known position.\nKey must be positive number and smaller than 4294967295\n" << "Key: ";
			std::cin >> key;

			std::cout << "\nName for stego image: ";
			std::cin >> stego_name;


			unsigned int data_length = data.tellg();
			data.seekg(std::fstream::beg);
			unsigned char* data_buffer = new unsigned char[data_length];
			data.read(reinterpret_cast<char*> (data_buffer), data_length);
			data.close();
			if (key == 0) {
				std::srand(std::time(0));
				key = std::rand() % std::numeric_limits<unsigned int>::max();
				bm->bmHeader->reserved1 = (char16_t)key / (1 << 16);
				bm->bmHeader->reserved2 = key - bm->bmHeader->reserved1*(1 << 16);
				bool ok = bm->randomEmbedd(key, data_buffer, data_length);
				delete[] data_buffer;
				if (ok) {
					bm->save(stego_name);
					std::cout << "\nTotal bytes encoded to the image: " << data_length;
					std::cout << "\nUsed key: " << key;
					std::cout << "\nPress \"enter\" to quit";
					delete bm;
					std::cin.ignore(); std::cin.get();
				}
				else {
					std::cout << "\nPress \"enter\" to quit";
					std::cin.ignore(); std::cin.get();
					exit(1);
				}
			}
			else {
				bool ok = bm->randomEmbedd(key, data_buffer, data_length);
				delete[] data_buffer;
				if (ok) {
					bm->save(stego_name);
					std::cout << "\nTotal bytes encoded to the image: " << data_length;
					std::cout << "\nUsed key: " << key;
					std::cout << "\nPress \"enter\" to quit";
					delete bm;
					std::cin.ignore(); std::cin.get();
				}
				else {
					std::cout << "\nPress \"enter\" to quit";
					std::cin.ignore(); std::cin.get();
					exit(1);
				}
			}
		}
		else {
			std::cout << "\nFailed to open file." << "\nPress \"enter\" to quit";
			std::cin.ignore(); std::cin.get();
		}
	}
	else {
		std::cout << "\nPress \"enter\" to quit";
		std::cin.ignore(); std::cin.get();

	}
}


void decodeFunc() {
	std::string cover_name;
	std::string stego_name;
	unsigned int key = 0;
	unsigned int bytes;
	std::cout << "\nName for stego image: ";
	std::cin >> stego_name;
	Steg::Bitmap* bm = Steg::readBitmap(stego_name);
	if (bm != NULL) {
		std::cout << "\nBytes to look for: ";
		std::cin >> bytes;

		std::cout << "\nKey required for decoding, if 0 is given the program tries to look for key from pre-known position in image file.\nKey must be positive number and smaller than 4294967295\n" << "Key: ";
		std::cin >> key;

		std::cout << "\nName for file to be extracted from the image: ";
		std::cin >> cover_name;


		unsigned char* data_buffer = new unsigned char[bytes];
		if (key == 0) {
			key = bm->bmHeader->reserved1*(1 << 16);
			key += bm->bmHeader->reserved2;
			data_buffer = bm->decode(key, bytes);
			std::ofstream os(cover_name, std::ios_base::binary);
			os.write(reinterpret_cast<char*>(data_buffer), bytes);
			os.close();
			delete[] data_buffer;
			std::cout << "\nDecoded " << bytes << " bytes with key: " << key;
			std::cout << "\nPress \"enter\" to quit";
			delete bm;
			std::cin.ignore(); std::cin.get();
			exit(0);

		}
		else {
			data_buffer = bm->decode(key, bytes);
			std::ofstream os(cover_name, std::ios_base::binary);
			os.write(reinterpret_cast<char*>(data_buffer), bytes);
			os.close();
			delete[] data_buffer;
			std::cout << "\nDecoded " << bytes << " bytes with key: " << key;
			std::cout << "\nPress \"enter\" to quit";
			delete bm;
			std::cin.sync(); std::cin.get();
			exit(0);
		}
	}
	else {
		std::cout << "\nPress \"enter\" to quit";
		std::cin.ignore(); std::cin.get();
		exit(1);
	}
}

void analyseFunc() {
	std::string stego_name;
	std::cout << "\nName for stego image: ";
	std::cin >> stego_name;
	Steg::Bitmap* bm = Steg::readBitmap(stego_name);

	if (bm != NULL) {
		bool result = bm->analyze();
		if (result) {
			std::cout << "\nImage has been modified";
			std::cout << "\nPress \"enter\" to quit";
			delete bm;
			std::cin.ignore(); std::cin.get();
			exit(0);
		}
		else {
			std::cout << "\nImage has NOT been modified";
			std::cout << "\nPress \"enter\" to quit";
			delete bm;
			std::cin.ignore(); std::cin.get();
			exit(0);
		}
	}
	else {
		std::cout << "\nPress \"enter\" to quit";
		std::cin.ignore(); std::cin.get();
		exit(1);
	}
}