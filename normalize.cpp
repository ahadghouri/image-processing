#define NUMTHREADS 4

double sequential_normalize(unsigned char *inputImage, unsigned char* outputImage, int width, int height)
{
	int index;
	int n = width*height;
	float mean = 0.0, var = 0.0, svar, std;
	double startNormalizeSequential = omp_get_wtime();
	for (index = 0; index < n; index++) {
		mean += (float)(inputImage[index]);
	}
	mean /= (float)n;
	for (index = 0; index < n; index++) {
		svar = (float)(inputImage[index]) - mean;
		var += svar*svar;
	}
	var /= (float)n;
	std = sqrtf(var);
	for(index = 0; index < n; index++) {
		outputImage[index] = (inputImage[index] - mean) /std;
	}
	double endNormalizeSequential  = omp_get_wtime();

	return endNormalizeSequential - startNormalizeSequential;
}

double parallel_normalize(unsigned char *inputImage, unsigned char* outputImage, int width, int height, int channels)
{
	int rowTask = height / NUMTHREADS;
	int index;
	int n = width*height*channels;
	float mean = 0.0, var = 0.0, svar, std;
	// Calculate the mean of the image intensities
	double startNormalizeParallel = omp_get_wtime();
	#pragma omp parallel for num_threads(NUMTHREADS) \
	shared(inputImage, n) reduction(+:mean) \
	private(index) schedule(static)
	for (index = 0; index < n; index++) {
		mean += (float)(inputImage[index]);
	}
	mean /= (float)n;
	// Calculate the standard deviation of the image intensities
	#pragma omp parallel for num_threads(NUMTHREADS) \
	shared(inputImage, n) reduction(+:var) \
	private(index, svar) schedule(static)
	for (index = 0; index < n; index++) {
		svar = (float)(inputImage[index]) - mean;
		var += svar*svar;
	}
	var /= (float)n;
	std = sqrtf(var);
	// Rescale using the calculated mean and standard deviation
	#pragma omp parallel for num_threads(NUMTHREADS) \
	shared(inputImage, outputImage, n) private(index) \
	firstprivate(mean, std) 
	
		for(index = 0; index < height*width*channels; index++) {
			outputImage[index] = (inputImage[index] - mean) /std;
		}
	
	double endNormalizeParallel = omp_get_wtime();	
	return endNormalizeParallel - startNormalizeParallel;
}
void parallel_normalizeLocalResult(unsigned char *inputImage, unsigned char* outputImage, int width, int height, int channels)
{
	int rowTask = height / NUMTHREADS;
	int index;
	int n = width*height*channels;
	float mean = 0.0, var = 0.0, svar, std;
	// Calculate the mean of the image intensities
	#pragma omp parallel for num_threads(NUMTHREADS) \
	shared(inputImage, n) reduction(+:mean) \
	private(index) schedule(static)
	for (index = 0; index < n; index++) {
		mean += (float)(inputImage[index]);
	}
	mean /= (float)n;
	// Calculate the standard deviation of the image intensities
	#pragma omp parallel for num_threads(NUMTHREADS) \
	shared(inputImage, n) reduction(+:var) \
	private(index, svar) schedule(static)
	for (index = 0; index < n; index++) {
		svar = (float)(inputImage[index]) - mean;
		var += svar*svar;
	}
	var /= (float)n;
	std = sqrtf(var);
	// Rescale using the calculated mean and standard deviation
	#pragma omp parallel num_threads(NUMTHREADS) \
	shared(inputImage, outputImage, n) private(index) \
	firstprivate(mean, std) 
	{
		int k = omp_get_thread_num();
		unsigned char *localArr = new unsigned char[rowTask * width * channels];
	 	for (int i = 0; i < rowTask*width*channels; i++)
	    {
	        localArr[i] = inputImage[i + k*rowTask*width*channels];
	    }
		
	    	if (k == 0)
		    {   
				stbi_write_png("normalize_input_LP0.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));          
			}
		    if (k == 1)
		    {
		        stbi_write_png("normalize_input_LP1.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
		    }
		    if (k == 2)
		    {
		        stbi_write_png("normalize_input_LP2.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
		    }
		    if (k == 3)
		    {
		        stbi_write_png("normalize_input_LP3.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
		    }	

		for(index = 0; index < rowTask*width*channels; index++) {
			localArr[index] = (inputImage[index + k*rowTask*width*channels] - mean) /std;
		}
	    	if (k == 0)
		    {   
				stbi_write_png("normalize_LP0.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));          
			}
		    if (k == 1)
		    {
		        stbi_write_png("normalize_LP1.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
		    }
		    if (k == 2)
		    {
		        stbi_write_png("normalize_LP2.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
		    }
		    if (k == 3)
		    {
		        stbi_write_png("normalize_LP3.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
		    }	
	    
	}
}
