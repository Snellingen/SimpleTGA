#include "../util/util.h"
#include <string.h>

typedef struct 
{	
	uint8_t idLength;
	uint8_t colorMapType;
	uint8_t dataTypeCode;
	uint16_t colorMapOrigin;
	uint16_t colorMapLength;
	uint8_t colorMapEntrySize;
	uint16_t x_origin;
	uint16_t y_origin;
	uint16_t height;
	uint16_t width;
	uint8_t imagePixelSize;
	uint8_t imageDescriptor;
}__attribute__((packed)) TGA_HEADER;

typedef struct _TgaFooter
{
  uint32_t extensionOffset;
  uint32_t dveloperOffset;
  char signature[18];        
}__attribute__((packed)) TGA_FOOT;

typedef struct
{
	unsigned char r, g, b, a;
}__attribute__((packed)) PIXEL32;
typedef struct
{
	unsigned char r, g, b;
}__attribute__((packed)) PIXEL24;

typedef struct
{
	uint16_t r : 5;
	uint16_t g : 5;
	uint16_t b : 5;
	uint16_t a : 1; 
}__attribute__((packed)) PIXEL16;

typedef struct 
{
	uint8_t luma; 
}__attribute__((packed)) PIXEL8;

typedef union 
{
	PIXEL32 *pixel32;
	PIXEL24 *pixel24;
	PIXEL16 *pixel16;
	PIXEL8 *pixel8;
}PIXEL_TYPE;

typedef struct 
{
	TGA_HEADER header;
	int imageIdSize; 
	char *imageIDfield; 
	int colorMapSize;  
	PIXEL_TYPE colorMap;
	PIXEL_TYPE pixels;
	int pixelCount;
	PIXEL8 *lumaData; 
	int hasFootFlag;
	int hasColorMapFlag;
	TGA_FOOT foot; 
}TGA_FILE;

typedef enum  
{
	HORIZONTAL = 1,
	VERTICAL = 2,
}FLIP;

int loadTGA(char *path, TGA_FILE *imageFile, int debug);
int readFile(FILE *inputFile, TGA_FILE *imageFile, int debug);
int convertToGrayScale(TGA_FILE *imageFile);
int writeFile(char * path, TGA_FILE *imageFile);
void flipImage(TGA_FILE *imageFile, FLIP flip);

