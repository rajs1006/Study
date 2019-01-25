//============================================================================
// Name        : Dip4.h
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : header file for fourth DIP assignment
//============================================================================

#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Dip4{

   public:
      // constructor
      Dip4(void){};
      // destructor
      ~Dip4(void){};
        
      // processing routines
      // start image restoration
      Mat run(const Mat& in, string restorationType, const Mat& kernel, double snr=pow(10,5));
      // testing routine
      void test(void);
      // function headers of given functions
      Mat degradeImage(const Mat& img, Mat& degradedImg, double filterDev, double snr);
      void showImage(const char* win, const Mat& img, bool cut=true);

   private:
      // function headers of functions to be implemented
      // --> please edit ONLY these functions!
      Mat inverseFilter(const Mat& degraded, const Mat& filter);
      Mat wienerFilter(const Mat& degraded, const Mat& filter, double snr);

      // function headers of functions implemented in previous exercises
      // --> re-use your (corrected) code
      Mat circShift(const Mat& in, int dx, int dy);
      Mat frequencyConvolution(const Mat& in, const Mat& kernel);
    
      // testing routines
      void test_circShift(void);
};
