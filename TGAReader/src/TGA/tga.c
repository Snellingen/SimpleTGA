#include "tga.h"

int loadTGA(char *path, TGA_FILE *imageFile, int debug)
{
	FILE *file = fopen(path, "r");
	if (file == NULL) 
 	{
 		fprintf(stderr, "Could not open file\n");
 		return 0; 
 	}	
 	if(!readFile(file, imageFile, debug))
 	{ 
 		fclose(file);
 		return 0;
 	} 
 	fclose(file);
	return 1;
}

int readCompressedPixels(FILE *inputFile, TGA_FILE *imageFile)
{
	uint8_t readChunk[5]; 
	for (int i = 0; i < imageFile -> pixelCount; i++)
	{	
		fread(&readChunk, (imageFile -> header.imagePixelSize >> 3) + 1, 1, inputFile);
		uint8_t blockLength = readChunk[0] & 0x7f;
		memcpy((void*)(imageFile -> pixels.pixel32) +i * (imageFile -> header.imagePixelSize >> 3), readChunk+1, imageFile -> header.imagePixelSize >> 3);
		if (readChunk[0] & 0x80)
		{
			// chunk is compressed
			for (int j = 0; j < (blockLength); j++)
			{
				i++;
				memcpy((void*)(imageFile -> pixels.pixel32) + (i * (imageFile -> header.imagePixelSize >> 3)), readChunk+1, imageFile -> header.imagePixelSize >> 3);
			}
		}
		else 
		{
			for (int j = 0; j < (blockLength); j++)
			{
				fread(&readChunk, (imageFile -> header.imagePixelSize >> 3), 1, inputFile);
				i++;
				memcpy((void*)(imageFile -> pixels.pixel32) + i * (imageFile -> header.imagePixelSize >> 3), readChunk, imageFile -> header.imagePixelSize >> 3);
			}
		}
	}
	return 1; 
}

int readFile(FILE *inputFile, TGA_FILE *imageFile, int debug)
{	 
	// Reading header
	if(!fread(&(imageFile -> header), sizeof(TGA_HEADER), 1, inputFile)) 
	{
		fprintf(stderr, "Unable to read header or filetype is unsupportet. \n");
		return 0;	
	}
	imageFile -> pixelCount =  imageFile -> header.height * imageFile -> header.width;
	
	// Reading imageId
	imageFile -> imageIDfield = NULL; 
	if(imageFile -> header.idLength != 0)
	{
		imageFile -> imageIDfield = malloc(imageFile -> header.idLength + 1);
		fread(imageFile -> imageIDfield, imageFile -> header.idLength, 1, inputFile);
	}
	
	// Get colorMap
	imageFile -> colorMapSize = 0; 
	if((imageFile -> header.dataTypeCode & 0x03) == 1)
	{
		imageFile -> colorMapSize = (imageFile -> header.colorMapEntrySize >> 3) * imageFile -> header.colorMapLength; 
		//fseek(inputFile, imageFile -> colorMapSize, SEEK_CUR);
		imageFile -> colorMap.pixel32 = malloc(imageFile -> colorMapSize);
		fread(imageFile -> colorMap.pixel32, imageFile -> colorMapSize, 1, inputFile);
	}
	
	// reading image data
	//fseek(inputFile, sizeof(TGA_HEADER) + (imageFile -> header.idLength) + imageFile, SEEK_SET);
	imageFile -> pixels.pixel32 = malloc((imageFile -> header.imagePixelSize >> 3) * imageFile -> pixelCount);
	imageFile -> lumaData = malloc(sizeof(PIXEL8) * imageFile -> pixelCount);
	
	if(imageFile -> header.dataTypeCode & 0x08)
		readCompressedPixels(inputFile, imageFile);
		
	else
		fread(imageFile -> pixels.pixel32, (imageFile -> header.imagePixelSize >> 3), imageFile -> pixelCount, inputFile);
	
	// Reading footer
	fseek(inputFile, -26, SEEK_END);
	fread(&(imageFile -> foot), 26, 1, inputFile);
	imageFile -> hasFootFlag = 0; 
	if (GetHash(imageFile -> foot.signature) == GetHash("TRUEVISION-XFILE."))
	{
		imageFile -> hasFootFlag = 1;
		if (debug)
			printf("File has footer!\n");
	}
	
	if (debug)
	{
		printf("idLength: %d \n", imageFile -> header.idLength);
		printf("colorMapData: %d \n", imageFile -> header.colorMapType);
		printf("dataTypeCode: %d \n", imageFile -> header.dataTypeCode);
		printf("colorMapOrigin: %d \n", imageFile -> header.colorMapOrigin);
		printf("colorMapLength: %d \n", imageFile -> header.colorMapLength);
		printf("colorMapEntrySize: %d \n", imageFile -> header.colorMapEntrySize);
		printf("x_origin: %d \n", imageFile -> header.x_origin);
		printf("y_origin: %d \n", imageFile -> header.y_origin);
		printf("width: %d \n", imageFile -> header.width);
		printf("height: %d \n", imageFile -> header.height);
		printf("imagePixelSize: %d \n", imageFile -> header.imagePixelSize);
		printf("imageDescriptor: %02X \n", imageFile -> header.imageDescriptor);
		printf("imageIDfield: %s \n", imageFile -> imageIDfield);
		printf("imageIdSize: %d \n", imageFile -> header.idLength);
	}
	
	return 1; 
}

void convertToGrayScale32(PIXEL_TYPE *px, int pixelCount, PIXEL8 *lumaData)
{
	float luma = 0;
	for (int i = 0; i < pixelCount; i++)
	{
		luma = 0.299 * (*px).pixel32[i].r + 0.587 * (*px).pixel32[i].g + 0.114 * (*px).pixel32[i].b;
		if (luma > 255) luma = 255;
		uint8_t intluma = luma; 
		(*px).pixel32[i].r = (*px).pixel32[i].g = (*px).pixel32[i].b = lumaData[i].luma = intluma;
	}
}

void convertToGrayScale24(PIXEL_TYPE *px, int pixelCount, PIXEL8 *lumaData)
{
	float luma = 0; 
	for (int i = 0; i < pixelCount; i++)
	{
		luma = 0.299 * (*px).pixel24[i].r + 0.587 * (*px).pixel24[i].g + 0.114 * (*px).pixel24[i].b;
		if (luma > 255) luma = 255;
		uint8_t intluma = luma; 
		(*px).pixel24[i].r = (*px).pixel24[i].g = (*px).pixel24[i].b = lumaData[i].luma = intluma;
	}
}

void convertToGrayScale16(PIXEL_TYPE *px, int pixelCount, PIXEL8 *lumaData)
{
	float luma = 0; 
	for (int i = 0; i < pixelCount; i++)
	{
		luma = 0.299 * ((*px).pixel16[i].r << 3) + 0.587 * ((*px).pixel16[i].g << 3) + 0.114 * ((*px).pixel16[i].b << 3);
		if (luma > 255) luma = 255;
		uint8_t intluma = luma; 
		lumaData[i].luma = intluma;
		if (luma == 255) intluma = 31;
		(*px).pixel16[i].r = (*px).pixel16[i].g = (*px).pixel16[i].b = intluma; // lumadata?
	}
}

int convertToGrayScale(TGA_FILE *imageFile)
{ 	
	int pixelDepth = imageFile -> header.imagePixelSize; 
	PIXEL_TYPE *px = &(imageFile -> pixels);
	int pixelCount = imageFile -> pixelCount;
	PIXEL8 *lumaData = imageFile -> lumaData;
	/* I added support for non 8bit grayscale, that is why I have own fuctions for them
	 	if I made my program only support to save as 8bit, I would not need to make a fuction for each type.
	 	I wanted my program to support save as original bit depth, so that's why I did it like this */
	
	if(imageFile -> colorMapSize > 0)
	{
		pixelDepth = imageFile -> header.colorMapEntrySize; 
		px = &(imageFile -> colorMap);
		pixelCount = imageFile -> header.colorMapLength;
	}
	
	switch(pixelDepth)
	{
		case 32: convertToGrayScale32(px, pixelCount, lumaData);break; 
		case 24: convertToGrayScale24(px, pixelCount, lumaData);break; 
		case 16: convertToGrayScale16(px, pixelCount, lumaData);break; 
		case 8: lumaData = (*px).pixel8; break;
		default: fprintf(stderr,"File with %d pixel depth is not yet supported for read.\n", pixelDepth);
		return 0;
	}
	return 1; 
}

void flipImage(TGA_FILE *imageFile, FLIP flip)
{
	if (flip == HORIZONTAL)
		imageFile -> header.imageDescriptor ^= 0x20;
	if (flip == VERTICAL)
    	imageFile -> header.imageDescriptor ^= 0x10;
}

int writeFile(char * path, TGA_FILE *imageFile)
{
	FILE *newFile = fopen(path, "w");
	fwrite(&(imageFile -> header), sizeof(TGA_HEADER), 1, newFile);
	
	if (imageFile -> header.idLength)
		fwrite(imageFile -> imageIDfield, imageFile -> header.idLength, 1, newFile);
	
	if (imageFile -> colorMapSize != 0)
	{
		fwrite(imageFile -> colorMap.pixel32, imageFile -> colorMapSize, 1, newFile);
	}
		
	fwrite(imageFile -> pixels.pixel32, imageFile -> header.imagePixelSize >> 3, imageFile -> pixelCount, newFile);
	
	if (imageFile -> hasFootFlag)
		fwrite(&(imageFile -> foot), sizeof(TGA_FOOT), 1, newFile);
		
	fclose(newFile);
	return 1; 
}
