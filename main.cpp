#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include "stb_image.h"
#include "stb_image_write.h"
#include <omp.h>
#include <unistd.h>
#include <cmath>
#include <stdlib.h>

#include "Warp.cpp"
#include "normalize.cpp"
#include "colorInversion.cpp"
#define NUMTHREADS 4

using namespace std;

int selectImage()
{
	int choice;
	do{
		cout<<"\t***SELECT YOUR IMAGE***"<<endl << endl;
	    cout<<"1. SHREK  \t[ 225 x 225   ]"<<endl;
	    cout<<"2. Design \t[ 1998 x 2000 ]"<<endl;
	    cout<<"3. Scenery\t[ 455 x 728   ]"<<endl;
	    cout<<"4. Sunset \t[ 2000 x 3000 ]"<<endl;
	    cout<<"5. Bot    \t[ 1159 x 2000 ]"<<endl;
	    cout<<"6. Go back to home page"<<endl;
	    cout << endl << "Enter your choice: ";
	    cin >> choice;
	    if(choice != 1 && choice!= 2 && choice !=3 && choice !=4 && choice !=5 && choice !=6){
	    	cout << "Invalid choice entered";
	    	sleep(1);
		}
		system("cls");
	}while(choice != 1 && choice!= 2 && choice !=3 && choice !=4 && choice !=5 && choice !=6);
    
	return choice;
}

int homePage()
{
	int choice;
	do{
		cout << "\t***HOME PAGE***" << endl << endl;
		cout << "1. Select Image" << endl;
		cout << "2. Exit Program" << endl;
		cout << "Enter your choice: ";
		cin >> choice;
		
		if(choice != 1 && choice != 2){
			cout << "Invalid choice entered!";
		}
		sleep(1);
		system("cls");
	}while(choice != 1 && choice != 2);
}

int Menu()
{
    int choice;
    
    do{
    	cout<<"\n\t***SELECT APPLICTAIONS OF IMAGE PROCESSING***"<<endl;
	    cout<<"1. Color Inversion "<<endl;
	    cout<<"2. Image Warping "<<endl;
	    cout<<"3. Normalization "<<endl;
	    cout<<"4. Go back to home page "<<endl;
	    cout<<"Enter your choice: ";
	    cin >> choice;
	    if(choice != 1 && choice!= 2 && choice !=3 && choice!=4){
	    	cout << "Invalid choice entered!" << endl;
	    	sleep(1);
			system("cls");
		}
		
	}while(choice != 1 && choice!= 2 && choice !=3 && choice!=4);

    return choice;
}

int main()
{
	int homeChoice, processChoice, imageChoice;
	string filename;
	while(1){
		homeChoice = homePage();
		if(homeChoice == 2){
			break;
		}
		if(homeChoice == 1){
			imageChoice = selectImage();
			int width, height, channels;
			unsigned char *data;
			if(imageChoice == 1)
		    {
		    	data = stbi_load("img_shrek225.jpg", &width, &height, &channels, 0);
				filename = "Shrek";
			}
			else if(imageChoice == 2)
		    {
		    	data = stbi_load("img_Design.png", &width, &height, &channels, 0);
				filename = "Design";
			}
			else if(imageChoice == 3)
		    {
		    	data = stbi_load("img_scenery.jpg", &width, &height, &channels, 0);
				filename = "Scenery";
			}
			else if(imageChoice == 4)
		    {
		    	data = stbi_load("img_sunset3000.jpg", &width, &height, &channels, 0);
				filename = "Sunset";
			}
			else if(imageChoice == 5)
		    {
		    	data = stbi_load("img_bot1200.png", &width, &height, &channels, 0);
				filename = "Bot";
			}
			if(data == NULL){
		    	cout << "Failed to load image" << endl;
		    	exit(1);
			}
			x:
			cout << "\t***IMAGE INFORMATION***" << endl << endl;
			cout<< "FILENAME: "<< filename;
			cout << "\nROWS: " << height;
			cout << "\nWIDTH: " << width;
			cout << "\nCOMPONENTS: " << channels;
			cout << "\nSIZE: " << height*width*channels << endl << endl;
			
			int size =  width*height*channels;
			short **pixelArr = new short*[height];
			short **outputArr = new short*[height];
		 
		    for (int i = 0; i < height; i++) {
		 
		        pixelArr[i] = new short[width*channels];
		        outputArr[i] = new short[width*channels];
		    }
		 
			for(int i=0; i<height; i++){
				for(int j=0; j<width*channels; j++){
					pixelArr[i][j] = short(data[i*width*channels + j]);
				}
			}
		   int choice;
		   double Ptime,Stime;
		   
		   processChoice = Menu();
		   cout << endl << endl;
		   if(processChoice==1)
		    {
		        unsigned char * Coutput = new unsigned char [height * width *channels];
		        Stime = sequentialColorInversion(pixelArr, Coutput, height, width, channels);
		        printf("Color Inversion [ Sequential ] =  %lf seconds\n", Stime);
		        stbi_write_png("colorInversion_S.png", width, height, channels, Coutput, channels * width * sizeof(unsigned char));
		        
		        Ptime = parallelColorInversion(pixelArr, Coutput, height, width, channels);
		        printf("Color Inversion [ parallel ] =  %lf seconds\n", Ptime);
		        stbi_write_png("colorInversion_P.png", width, height, channels, Coutput, channels * width * sizeof(unsigned char));	
		       	
		        cout<<"Now Printing Local Results "<<endl;
		        parallelColorInversionLocalResult(pixelArr, Coutput, height, width, channels);
		    
		    }
		    
			else if(processChoice == 2)
		    {
				int  tx = width / 2, ty = height / 2;
			
			    unsigned char* imgData = new unsigned char[width*height];
			
			    if(channels ==3)
			    {
			        imgData = rgbToGray(pixelArr, width,height, channels);
			        stbi_write_png("grayscale.png", width, height, 1, imgData,  width * sizeof(unsigned char));	
			    }	
			
			    short **WarpArr = new short*[height];
			 
			    for (int i = 0; i < height; i++) {
			 
			        WarpArr[i] = new short[width];
			    }
			
			
				for(int i=0; i<height; i++){
					for(int j=0; j<width; j++){
						WarpArr[i][j] = short(imgData[i*width + j]);
					}
				}
		        
		        Stime = sequential_warping(WarpArr, outputArr, 30, tx, ty, height, width);
		        printf("Image Warping [ Sequential ] = %lf seconds\n", Stime);
		        unsigned char *SnewData = new unsigned char[height*width];
		        for(int i=0; i<height; i++){
		            for(int j=0; j<width; j++){
		                SnewData[i*width + j] = (unsigned char)outputArr[i][j];
		            }
		        }
		        stbi_write_png("warp_s.png", width, height, 1, SnewData, width * sizeof(unsigned char));
		
		        Ptime = parallel_warping(WarpArr, outputArr, 30, tx, ty, height, width);
		        printf("Image Warping [ Parallel ] = %lf seconds\n", Ptime);
		        unsigned char *PnewData = new unsigned char[height*width];
		        for(int i=0; i<height; i++){
		            for(int j=0; j<width; j++){
		                PnewData[i*width + j] = (unsigned char)outputArr[i][j];
		            }
		        }
		        stbi_write_png("warp_P.png", width, height, 1, PnewData, width * sizeof(unsigned char));
		        cout<<"Now Printing Local Results "<<endl;
		        parallel_warpingLocalResult(WarpArr, outputArr, 30, tx, ty, height, width);
		
		    }
		   else if(processChoice==3)
		    {
		         unsigned char* Soutput = new unsigned char[width*height*channels];
		         Stime= sequential_normalize(data, Soutput, width*channels, height);
		         printf("Normalization [ Sequential] = %lf seconds\n", Stime );
		         stbi_write_png("normalize_S.png", width, height, channels, Soutput, width * channels * sizeof(unsigned char));
		
		        unsigned char* Poutput = new unsigned char[width*height*channels];
		        
		        Ptime = parallel_normalize(data, Poutput, width, height, channels);
		        printf("Normalization [ Parallel ] = %lf seconds\n", Ptime);
		        stbi_write_png("normalize_P.png", width, height, channels, Poutput, width * channels * sizeof(unsigned char));
		        cout<<"Now Printing Local Results "<<endl;
		        parallel_normalizeLocalResult(data, Poutput, width, height, channels);
		
		    }
		    else if(processChoice==4){
		    	system("cls");
		        continue;
		    }
		    cout << endl << endl;
		    system("pause");
		    system("cls");
		    goto x;
		}
	}
	

    return 0;
}
