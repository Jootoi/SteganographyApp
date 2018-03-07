# Simple Steganography App

## What does it do?
It encodes data to images so that changes to image are invisible to human eye (and maybe to statistical analysis, but don't count on it). 
Data hidden to the image can be recovered using the decoding function. Program can also do very basic analysis to the image to determine if something is hidden in it.
Analysis method is based on J. Fridrich and M. Long, “Steganalysis of LSB Encoding in Color Images,” Proceedings of the IEEE
International Conference on Multimedia and Expo (ICME), vol. 3, pp. 1279 – 1282, New York,
NY, USA, July – August 2000. 

## How do I use it?
Launch the .exe and follow the orders.

Few notes though:
Only supported cover image format for now is 24 bits per pixel bitmap with Windows style bitmap header. This is the kind of bitmap that you get 
when you save your image in Microsoft paint using 24-bit bitmap option.

Maximum size for data to be hidden is about 1/8th of the size of the cover image. If you want to save more data you have to divide it to multiple files 
and hide those to different images (or use larger cover image).

Key used in encoding and decoding needs to be positive and smaller than 32-bit unsigned integer maximum value. If key given is 0 a pseudo-random key is generated
and written to image header as well as shown to user at end of encoding. When decoding and key = 0 is given the key will be read from the header.

Do not forget to give filename extension when giving filenames.

## License
 Copyright © 2000 Joonas Toimela <Toimela.Joonas.J@student.uta.fi>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.