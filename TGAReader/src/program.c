#include"program.h"
char *filePath = NULL; 
char *savePath = NULL; 
int grayScaleFlag = 0; 
int flipOrientation = 0; 
int saveAs8bitFlag = 0;
int debug = 0; 

int main(int argc, char **argv)
{	
	// Variabled and pointers
 	TGA_FILE imageFile;
 	TGA_FILE saveFile; 
 	
 	//get parameters
	parameterParser(argc, argv);
	
	if (filePath == NULL) fprintf(stderr, "Missing file path parameter.\n");
	 	
	// Load the TGA file
	if (!loadTGA(filePath, &imageFile, debug)) return 0;
	
	// convert to grayscale
	if(grayScaleFlag)
	{
		if(convertToGrayScale(&imageFile) == 0)
		{
			free(imageFile.pixels.pixel32);
			free(&(imageFile.lumaData -> luma));
				if (imageFile.imageIDfield != NULL) free(imageFile.imageIDfield);
			return 0; 
		}
	}
	
	// Create saveFile
	saveFile = imageFile;
	if(saveAs8bitFlag)
	{
		if (imageFile.colorMapSize == 0)
		{
			saveFile.header.imagePixelSize = 8; 
			saveFile.header.dataTypeCode = 3;
			saveFile.header.colorMapType = 0;
			saveFile.header.colorMapLength = 0;
			saveFile.header.colorMapEntrySize = 0; 
			saveFile.header.colorMapOrigin = 0; 
			saveFile.pixels.pixel8 = imageFile.lumaData;
		}
	}
	// flip image	
	if(flipOrientation > 0)
		flipImage(&saveFile, flipOrientation);
	 	
	// Save the new TGA file
	if (savePath)
		writeFile(savePath, &saveFile);
	else fprintf(stderr, "You need to enter a output path. ex: -o mysavepath\n"); 

 	// Clean uo & exit
 	// TODO fix 8bit read free
	free(imageFile.pixels.pixel32);
	free(&(imageFile.lumaData -> luma));
	if (imageFile.imageIDfield != NULL) free(imageFile.imageIDfield);
	if (imageFile.colorMapSize > 0) free(imageFile.colorMap.pixel32);
	return 0; 
}

void parameterParser(int argc, char **argv)
{
	// Get parameters and set flags
	for (int i = 1; i < argc ; i++)
	{
		int hash = GetHash(argv[i]);
		if (argv[i][0] != '-')
			filePath = argv[i];
		else if (hash == GetHash("--bw-n8"))
			grayScaleFlag = 1; 
		else if (hash == GetHash("-fh"))
			flipOrientation = HORIZONTAL;
		else if (hash == GetHash("-fv"))
			flipOrientation = VERTICAL;
		else if (hash == GetHash("-debug"))
			debug = 1; 
		else if (hash == GetHash("-o"))
			savePath = argv[++i];
		else if (hash == GetHash("-bw"))
		{
			saveAs8bitFlag = 1; 
			grayScaleFlag = 1; 
		}
		else 
		{
			// TODO make this automatic? 
			fprintf(stderr,"Invalid parameter: %s\n", argv[i]);
			printf("Available parameters are:\n'-bw' (black & white)\n");
			printf("'--bw-8bit' (flip vertical)\n");
			printf("'-fv' (flip vertical)\n");
			printf("'-fh' (flip horizontal)\n");
			printf("'-debug' (show debug output)\n");
			printf("'-o' followed by output path\n");
		}
	}
}
