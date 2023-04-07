#define NUMTHREADS 4
#include <omp.h>
#include <iostream>
using namespace std;
double sequentialColorInversion(short **input, unsigned char *output, int height, int width, int channels)
{
	double startClrInvSequential = omp_get_wtime();
    for (int i = 0; i < height; i++)
    {

        for (int j = 0; j < width * channels; j++)
        {

            output[i * width * channels + j] = (unsigned char)(255 - input[i][j]);
        }
    }
	double endClrInvSequential = omp_get_wtime();
	return endClrInvSequential - startClrInvSequential;
}

void parallelColorInversionLocalResult(short **input, unsigned char *output, int height, int width, int channels)
{
    int rowTask = height / NUMTHREADS;
#pragma omp parallel num_threads(NUMTHREADS)
    {
    	
        int index = omp_get_thread_num();
        unsigned char *localArr = new unsigned char[rowTask * width * channels];
        for (int i = 0; i < rowTask; i++)
        {
            for (int j = 0; j < width * channels; j++)
            {
                localArr[i * width * channels + j] = (unsigned char)(input[i + rowTask * index][j]);
            }
        }
        
        if (index == 0)
        {   
			stbi_write_png("colorInversion_input_LP0.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));          
		}
        if (index == 1)
        {
            stbi_write_png("colorInversion_input_LP1.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        if (index == 2)
        {
            stbi_write_png("colorInversion_input_LP2.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        if (index == 3)
        {
            stbi_write_png("colorInversion_input_LP3.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        
	    
        for (int i = 0; i < rowTask; i++)
        {
            for (int j = 0; j < width * channels; j++)
            {
                localArr[i * width * channels + j] = (unsigned char)(255 - input[i + rowTask * index][j]);
            }
        }
        
        
        
        
        if (index == 0)
        {   
			stbi_write_png("colorInversion_LP0.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));          
		}
        if (index == 1)
        {
            stbi_write_png("colorInversion_LP1.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        if (index == 2)
        {
            stbi_write_png("colorInversion_LP2.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        if (index == 3)
        {
            stbi_write_png("colorInversion_LP3.png", width, rowTask, channels, localArr, channels * width * sizeof(unsigned char));
        }
        
    }
}

double parallelColorInversion(short **input, unsigned char *output, int height, int width, int channels)
{
	
    int rowTask = height / NUMTHREADS;
    double time=0.0;
    double start = omp_get_wtime();
#pragma omp parallel for num_threads(NUMTHREADS) schedule(static)
		for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width * channels; j++)
            {
                output[i * width * channels + j] = (unsigned char)(255 - input[i][j]);
            }
        }
    double end = omp_get_wtime();
    time = end - start;
    return time;
    
}




