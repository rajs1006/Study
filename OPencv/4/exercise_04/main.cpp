//============================================================================
// Name        : main.cpp
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : only calls processing and test routines
//============================================================================

#include <iostream>

#include "Dip4.h"

using namespace std;

// usage: path to image in argv[1], SNR in argv[2], stddev of Gaussian blur in argv[3]
// main function. Loads the image, calls test and processing routines, records processing times
int main(int argc, char** argv) {

   // check if enough arguments are defined
   if (argc < 4){
      cout << "Usage:\n\tdip4 path_to_original snr stddev"  << endl;
      cout << "\t\t snr :\t\tsignal-to-noise ratio: the higher (e.g. 10,000), the less noise." << endl;
      cout << "\t\t stddev :\tstddev of Gaussian blur" << endl;
      cout << "Press enter to exit"  << endl;
      cin.get();
      return -1;
   }

   // construct processing object
   Dip4 dip4;

   // run test routines
   // NOTE: uncomment the following line for debugging/testing purposes!
   dip4.test();
   
	// some windows for displaying imagesvoid degradeImage(Mat imgIn32F, Mat degradedImg, double filterDev, double snr)
    const char* win_1 = "Original Image";
    const char* win_2 = "Degraded Image";
    const char* win_3 = "Restored Image: Inverse filter";
    const char* win_4 = "Restored Image: Wiener filter";
    namedWindow( win_1 );
    namedWindow( win_2 );
    namedWindow( win_3 );
    namedWindow( win_4 );
   
    // load image, path in argv[1]
    cout << "load image" << endl;
    Mat img = imread(argv[1], 0);
    if (!img.data){
      cout << "ERROR: Cannot find original image"  << endl;
      cout << "Press enter to exit..."  << endl;
      cin.get();
      return -1;
    }
    // convert U8 to 32F
    img.convertTo(img, CV_32FC1);
    cout << " > done" << endl;

    // show and safe gray-scale version of original image
    dip4.showImage( win_1, img);
    imwrite( "original.png", img );
  
    // degrade image
    cout << "degrade image" << endl;
    double snr = atof(argv[2]);
    double filterDev = atof(argv[3]);
    Mat degradedImg;
    Mat gaussKernel = dip4.degradeImage(img, degradedImg, filterDev, snr);
    cout << " > done" << endl;
    
    // show and safe degraded image
    dip4.showImage( win_2, degradedImg);
    imwrite( "degraded.png", degradedImg );
   
    // inverse filter
    cout << "inverse filter" << endl;
    Mat restoredImgInverseFilter = dip4.run(degradedImg, "inverse", gaussKernel);
    cout << " > done" << endl;
    // show and safe restored image
    dip4.showImage( win_3, restoredImgInverseFilter);
    imwrite( "restored_inverse.png", restoredImgInverseFilter );
    
    // wiener filter
    cout << "wiener filter" << endl;
    Mat restoredImgWienerFilter = dip4.run(degradedImg, "wiener", gaussKernel, snr);
    cout << " > done" << endl;
    // show and safe restored image
    dip4.showImage( win_4, restoredImgWienerFilter, false);
    imwrite( "restored_wiener.png", restoredImgWienerFilter );

    // wait
    waitKey(0);

   return 0;
} 
