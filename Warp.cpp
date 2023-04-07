#define M_PI 3.14159265358979323846
#define FILL 150
#define NUMTHREADS 4


unsigned char *rgbToGray(short **arr, int w, int h, int c){
	
	unsigned char *result = new unsigned char[w*h];
	int k=0;
	int index=0;
	for(int i=0; i<h; i++){
		for(int j=0; j<w*c; j+=c){
			float avg = (arr[i][j+0] + arr[i][j+1] + arr[i][j+2])/3;
			result[index] = avg;
			index++;
		}
	}
	return result;
}

float BilinearlyInterpolate (short  **the_image, float x, float y, int rows, int cols)
{
	double fraction_x, fraction_y,
	one_minus_x, one_minus_y,
	tmp_double;
	int ceil_x, ceil_y, floor_x, floor_y;
	short p1, p2, p3, result = FILL;
	
	if(x < 0.0 || x >= (double)(cols-1) || y < 0.0 || y >= (double)(rows-1))
		return(result);
		
	tmp_double = floor(x);
	floor_x = tmp_double;
	tmp_double = floor(y);
	floor_y = tmp_double;
	tmp_double = ceil(x);
	ceil_x = tmp_double;
	tmp_double = ceil(y);
	ceil_y = tmp_double;
	fraction_x = x - floor(x);
	fraction_y = y - floor(y);
	one_minus_x = 1.0 - fraction_x;
	one_minus_y = 1.0 - fraction_y;
	tmp_double = one_minus_x * (double)(the_image[floor_y][floor_x]) + fraction_x * (double)(the_image[floor_y][ceil_x]);
	p1 = tmp_double;
	tmp_double = one_minus_x * (double)(the_image[ceil_y][floor_x]) + fraction_x * (double)(the_image[ceil_y][ceil_x]);
	p2 = tmp_double;
	tmp_double = one_minus_y * (double)(p1) + fraction_y * (double)(p2);
	p3 = tmp_double;
	return(p3);
}

double sequential_warping(short **the_image, short **out_image, float angle, int m, int n, int rows, int cols){
	double cosa, sina, radian_angle, tmpx, tmpy;
	int i, j, new_i, new_j;
	radian_angle = angle/57.29577951;
	cosa = cos(radian_angle);
	sina = sin(radian_angle);
	float theta, radius;

	double startWarpSequential = omp_get_wtime();
	for(i=0; i<rows; i++){
		for(j=0; j<cols; j++){		
				radius = sqrtf((j - m) * (j - m) + (i - n) * (i - n));
				theta = (radius / 30) * radian_angle;
				tmpx = cos(theta) * (j - m) - sin(theta) * (i - n) + m;
				tmpy = sin(theta) * (j - m) + cos(theta) * (i - n) + n;
			out_image[i][j] = BilinearlyInterpolate(the_image, tmpx, tmpy, rows, cols);
		} 
	}
	double endWarpSequential = omp_get_wtime();

	return endWarpSequential - startWarpSequential;
}


double parallel_warping(short **the_image, short **out_image, float angle, int m, int n, int rows, int cols){
	double cosa, sina, radian_angle, tmpx, tmpy;
	int i, j, new_i, new_j;
	cosa = cos(radian_angle);
	sina = sin(radian_angle);
	float theta, radius;
	
	int rowTask = rows/NUMTHREADS;
	double startWarpParallel = omp_get_wtime();
	#pragma omp parallel  num_threads(NUMTHREADS)\
	shared(the_image, out_image, rows, cols) \
	private(tmpx, tmpy, theta, radius, i, j)
	{
		int index=0;
		int k = omp_get_thread_num();
		radian_angle = (angle)/57.29577951;

		for(i=k*rowTask; i<k*rowTask + rowTask; i++){
			for(j=0; j<cols; j++){		
				radius = sqrtf((j - m) * (j - m) + (i - n) * (i - n));
				theta = (radius / 30) * (radian_angle);
				tmpx = cos(theta) * (j - m) - sin(theta) * (i - n) + m;
				tmpy = sin(theta) * (j - m) + cos(theta) * (i - n) + n;
				out_image[i][j]= BilinearlyInterpolate(the_image, tmpx, tmpy, rows, cols);
			} 
			index++;
		}	
	}
	double endWarpParallel = omp_get_wtime();
	return endWarpParallel - startWarpParallel;
}
void parallel_warpingLocalResult(short **the_image, short **out_image, float angle, int m, int n, int rows, int cols){
	double cosa, sina, radian_angle, tmpx, tmpy;
	int i, j, new_i, new_j;
	cosa = cos(radian_angle);
	sina = sin(radian_angle);
	float theta, radius;
	
	int rowTask = rows/NUMTHREADS;
	#pragma omp parallel  num_threads(NUMTHREADS)\
	shared(the_image, out_image, rows, cols) \
	private(tmpx, tmpy, theta, radius, i, j)
	{
		
		short **localArr = new short*[rowTask];
		for(int i=0; i<rowTask; i++){
			localArr[i] = new short[cols];
		}
		
		int index=0;
		int k = omp_get_thread_num();
		radian_angle = (angle)/57.29577951;

		   for (int i = 0; i < rowTask; i++)
        {
            for (int j = 0; j <cols; j++)
            {
                localArr[i][j] = (unsigned char)(the_image[i + rowTask * k][j]);
            }
        }

		
		 unsigned char *PnewData = new unsigned char[rowTask*cols];
		  for(int l=0; l<rowTask; l++){
            for(int j=0; j<cols; j++){
                PnewData[l*cols + j] = (unsigned char)localArr[l][j];
            }
        }
		
		if (k == 0)
        {   
			stbi_write_png("warp_input_LP0.png", cols, rowTask, 1, PnewData, 1 * cols * sizeof(unsigned char));          
		}
        if (k == 1)
        {
            stbi_write_png("warp_input_LP1.png", cols, rowTask, 1, PnewData, 1 * cols * sizeof(unsigned char));
        }
        if (k == 2)
        {
            stbi_write_png("warp_input_LP2.png", cols, rowTask, 1, PnewData, 1 * cols * sizeof(unsigned char));
        }
        if (k == 3)
        {
            stbi_write_png("warp_input_LP3.png", cols, rowTask, 1, PnewData, 1 * cols * sizeof(unsigned char));
        }
		

		for(i=k*rowTask; i<k*rowTask + rowTask; i++){
			for(j=0; j<cols; j++){		
				radius = sqrtf((j - m) * (j - m) + (i - n) * (i - n));
				theta = (radius / 30) * (radian_angle);
				tmpx = cos(theta) * (j - m) - sin(theta) * (i - n) + m;
				tmpy = sin(theta) * (j - m) + cos(theta) * (i - n) + n;
				localArr[index][j] = BilinearlyInterpolate(the_image, tmpx, tmpy, rows, cols);
			} 
			index++;
		}	
		
		  for(int l=0; l<rowTask; l++){
            for(int j=0; j<cols; j++){
                PnewData[l*cols + j] = (unsigned char)localArr[l][j];
            }
        }
		if (k == 0)
        {   
			stbi_write_png("warp_LP0.png", cols, rowTask, 1, PnewData, 1 * cols * sizeof(unsigned char));          
		}
        if (k == 1)
        {
            stbi_write_png("warp_LP1.png", cols, rowTask, 1, PnewData, 1 * cols * sizeof(unsigned char));
        }
        if (k == 2)
        {
            stbi_write_png("warp_LP2.png", cols, rowTask, 1, PnewData, 1 * cols * sizeof(unsigned char));
        }
        if (k == 3)
        {
            stbi_write_png("warp_LP3.png", cols, rowTask, 1, PnewData, 1 * cols * sizeof(unsigned char));
        }
	}
	
}



