//BinaryImage function definitions
#include "image.h"

//Check if header contains comments
//Comments start with #
bool isComment(string comment){

	for(unsigned int i = 0; i < comment.length(); i++){

		if(comment[i] == '#') return true;

		if(!isspace(comment[i])) return false;

	}

	return true;
}

//Reads binary pixel values in image
void BinaryImage::readImage(ifstream &inFile){

	//Check if the file stream in open
	if(!inFile){

		cerr << "Could not read from file!" << endl;

		exit(1000);

	}

	//Making a temp array, and putting it on the heap
	char * byteArray = new char[imageSize + 1];

	//Read the bytes of the image, and puts data in byteArray
	inFile.read(byteArray, imageSize);

	//If reading in the data failed, return an error
	if(inFile.fail()){

		cerr << "Error: cannot read pixels." << endl;

		exit(1000);

	}

	//Set the last element in array to EOF character
	byteArray[imageSize] = '\0';

	//Put the data read from file into pixels
	for(unsigned int i = 0; i < imageSize; i++){

		pixels.push_back(static_cast<int>
		(static_cast<unsigned char>(byteArray[i])));

	}

	//Delete the byteArray
	delete[] byteArray;

}

//Writes binary pixels to output file
void BinaryImage::writeImage(ofstream &outFile){

	//Check if the file stream is open
	if(!outFile){

		cerr << "Could not write to file." << endl;

		exit(1000);

	}

	//Write header
	outFile << "P5"       << " "  <<
			width         << " "  <<
			height        << " "  <<
			maxPixelValue << endl;

	//Take all pixel values from pixels and writes it to output file
	char * byteArray = new char[imageSize + 1];

	for(unsigned int i = 0; i < imageSize; i++){

		byteArray[i] = static_cast<char>(pixels[i]);

	}

	byteArray[imageSize] = '\0';

	outFile.write(byteArray, imageSize);

	if(outFile.fail()){

		cerr << "Error: error writing to file." << endl;

		exit(1000);

	}

	delete[] byteArray;

}

void AsciiImage::readImage(ifstream &inFile){

	//Check if the file opened properly
	if(!inFile){

		cerr << "Could not read from file." << endl;

		exit(1001);

	}

	int pixelValue;

	//Read in the Ascii values from file
	while(inFile >> pixelValue){

		pixels.push_back(pixelValue);

	}


}


void AsciiImage::writeImage(ofstream &outFile){

	//Check if file is open
	if(!outFile){

		cerr << "Could not write to file." << endl;

		exit(1001);

	}

	//Write Header
	outFile << "P2" << ' ' <<
			width << ' ' <<
			height << ' ' <<
			maxPixelValue << '\n';

	//Write the contents of pixels to the output file
	for(unsigned int i = 0; i < imageSize; i++){

		//Add a '\n' at the end of each row
		if(i % width == 0 && i != 0) outFile << '\n';

		outFile << pixels.at(i) << '\t';

	}

}

void Image::readHeader(ifstream &inFile){

	stringstream sStream;

	string line;

	//Check if the file opened successfully
	if(!inFile){

		cerr << "Error: Could not open file." << endl;

		exit(1002);

	}

	char readChar;

	string errorMessage = "Error: incorrect picture format.";

	getline(inFile, line);

	unsigned int lineSize = line.length();

	//After we read magic number, we read the next line and determine if it's valid
	for(unsigned int i = 0; i < lineSize; i++){

		if(!isspace(line[i])){

			cerr << errorMessage << endl;

			cerr << "Extra info after magic number." << endl;

			exit(1002);

		}

	}

	//Read through the rest of the header and skip through comments
	while(getline(inFile, line)){

		if(!(isComment(line))) break;

	}

	sStream << line;

	//Read in width.
	//If there is a problem, return error
	if(!(sStream >> width)){

		cerr << errorMessage << endl;

		cerr << "Cannot read width." << endl;

		exit(1002);

	}

	//Read in height
	//If there is a problem, return error
	if(!(sStream >> height)){

		cerr << errorMessage << endl;

		cerr << "Cannot read height." << endl;

		exit(1002);

	}

	//Check if there is extra information after width and height
	while(sStream >> readChar){

		if(!(isspace(readChar))){

			cerr << errorMessage << endl;

			cerr << "Extra info when reading height and width." << endl;

			exit(1002);

		}

	}

	//Make sure the height and width is positive
	if(width <= 0 || height <= 0){

		cerr << "Error: width and height cannot be negative" << endl;

		exit(1002);

	}

	//Check if there are any comments between height/width and maxPixelValue
	while(getline(inFile, line)){

		if(!(isComment(line))) break;

	}

	//Clear out the string stream
	sStream.str("");
	sStream.clear();

	sStream << line;

	//Read in the maxPixelValue
	if(!(sStream >> maxPixelValue)){

		cerr << errorMessage << endl;
		cerr << "Could not read maxPixelValue." << endl;

		exit(1002);

	}

	//Check if there is extra information after maxPixelValue
	while(sStream >> readChar){

		if(!(isspace(readChar))){

			cerr << errorMessage << endl;
			cerr << "Extra info after the max pixel value." << endl;

			exit(1002);

		}

	}

	if(maxPixelValue < 0 || maxPixelValue > 255){

		cerr << errorMessage << endl;
		cerr << "Invalid max pixel value." << endl;

		exit(1002);

	}

	imageSize = width * height;

}

//Finds the maxium pixel value in the image
void Image::findMax(){

	int maxVal = 0;

	for(unsigned int i = 0; i < imageSize; i++){

		if(pixels[i] > maxVal){

			maxVal = pixels[i];

		}

	}

	maxpix = maxVal;

}

//Finds the minimal pixel value of the image
void Image::findMin(){

	int minVal = 255;

	for(unsigned int i = 0; i < imageSize; i++){

		if(pixels[i] < minVal){

			minVal = pixels[i];

		}

	}

	minpix = minVal;

}

//Scales image so that the maximum pixel value is 255
void Image::scaleImage(){

	double calc = 0.0;

	int newPixelValue = 0;

	findMin();

	findMax();

	for(unsigned int i = 0; i < imageSize; i++){

		calc = (double)(pixels[i] - minpix) / (maxpix - minpix);

		newPixelValue = round(calc * 255);

		pixels.at(i) = newPixelValue;

	}

	maxPixelValue = 255;

}

//Sobel edge detection function - detects edges and draws an outline
void Image::edgeDection(){

	int x = 0, y = 0;

	int xG = 0, yG = 0;

	int * tempImage = new int[imageSize];

	for(unsigned int i = 0; i < imageSize; i++){

		x = i % width;

		if(i != 0 && x == 0){

			y++;

		}

		if(x < (width - 1) && y < (height - 1)
				&& (y > 0) && (x > 0)){

			//index = x + (y * width)
			//Finds the horizontal gradient
			xG = (pixels[(x+1) + ((y-1) * width)]
						 + (2 * pixels[(x+1) + (y * width)])
						 + pixels[(x+1) + ((y+1) * width)]
								  - pixels[(x-1) + ((y-1) * width)]
										   - (2 * pixels[(x-1) + (y * width)])
										   - pixels[(x-1) + ((y+1) * width)]);


			//Finds the vertical gradient
			yG = (pixels[(x-1) + ((y+1) * width)]
						 + (2 * pixels[(x) + ((y + 1) * width)])
						 + pixels[(x+1) + ((y+1) * width)]
								  - pixels[(x-1) + ((y-1) * width)]
										   - (2 * pixels[(x) + ((y-1) * width)])
										   - pixels[(x+1) + ((y-1) * width)]);

			//newPixel = sqrt(xG^2 + yG^2)
			tempImage[i] = sqrt((xG * xG) + (yG * yG));

		}else{

			//Pads out of bound pixels with 0
			tempImage[i] = 0;

		}

	}

	for(unsigned int i = 0; i < imageSize; i++){

		pixels[i] = tempImage[i];

	}


}


