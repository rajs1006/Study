//============================================================================
// Name        : Dip4.cpp
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : 
//============================================================================

#include "Dip4.h"

// Performes a circular shift in (dx,dy) direction
/*
in       :  input matrix
dx       :  shift in x-direction
dy       :  shift in y-direction
return   :  circular shifted matrix
*/
Mat Dip4::circShift(const Mat& in, int dx, int dy){
	Mat result = Mat(in.size(), CV_32FC1);
	for (int x = 0 ; x < in.cols ; x++){
		for (int y = 0 ; y < in.rows ; y++){
	      result.at<float>(y,x) = in.at<float>((y - dy) % in.rows, (x - dx) % in.cols);
	    }
	}
	return result;
}

// Function applies the inverse filter to restorate a degraded image
/*
degraded :  degraded input image
filter   :  filter which caused degradation
return   :  restorated output image
*/
Mat Dip4::inverseFilter(const Mat& degraded, const Mat& filter){
	Mat filterLarge = Mat::zeros(degraded.size(), CV_32FC1);
	filter.copyTo(filterLarge(Rect(0,0,filter.cols, filter.rows)));
	filterLarge = circShift(filterLarge, -filter.cols/2, -filter.rows/2);

	Mat degradedSpectrum = Mat(degraded.size(), CV_32FC1);
	Mat filterSpectrum = Mat(degraded.size(), CV_32FC1);

	dft(degraded, degradedSpectrum, DFT_COMPLEX_OUTPUT);
	dft(filterLarge, filterSpectrum, DFT_COMPLEX_OUTPUT);

	Mat out = Mat(degraded.size(), CV_32FC1);
	Mat q = Mat(degraded.size(), CV_32FC2);
	Mat pl[2];
	Mat magnitudes = Mat(degraded.size(), CV_32FC1);
	split(filterSpectrum,pl);
	magnitude(pl[0], pl[1],magnitudes);

	double max, min;
	minMaxLoc(magnitudes, &min, &max);
	double threshold = max * 0.05;

	for(int x = 0; x < degraded.cols; x++)
	{
		for(int y = 0; y < degraded.rows; y++)
		{
			if (magnitudes.at<float>(y,x) < threshold)
			{
				q.at<Vec2f>(y,x)[0] = 1.0f / threshold;
				q.at<Vec2f>(y,x)[1] = 0.0f;
			}
			else
			{
				float real = filterSpectrum.at<Vec2f>(y,x)[0];
				float imga = filterSpectrum.at<Vec2f>(y,x)[1];
				// divide 1 by the complex value (real + imga * i)
				q.at<Vec2f>(y,x)[0] = real / (pow(real,2) + pow(imga,2));
				q.at<Vec2f>(y,x)[1] = -imga / (pow(real,2) + pow(imga,2));
			}
		}
	}

	mulSpectrums(degradedSpectrum, q, out, 0);

	dft(out, out, DFT_REAL_OUTPUT + DFT_SCALE + DFT_INVERSE);

	return out;
}

// Function applies the wiener filter to restorate a degraded image
/*
degraded :  degraded input image
filter   :  filter which caused degradation
snr      :  signal to noise ratio of the input image
return   :   restorated output image
*/
Mat Dip4::wienerFilter(const Mat& degraded, const Mat& filter, double snr){
	Mat filterLarge = Mat::zeros(degraded.size(), CV_32FC1);
	filter.copyTo(filterLarge(Rect(0,0,filter.cols, filter.rows)));
	filterLarge = circShift(filterLarge, -filter.cols/2, -filter.rows/2);

	Mat degradedSpectrum = Mat(degraded.size(), CV_32FC1);
	Mat filterSpectrum = Mat(degraded.size(), CV_32FC1);

	dft(degraded, degradedSpectrum, DFT_COMPLEX_OUTPUT);
	dft(filterLarge, filterSpectrum, DFT_COMPLEX_OUTPUT);

	Mat out = Mat(degraded.size(), CV_32FC1);
	Mat q = Mat(degraded.size(), CV_32FC2);
	Mat pl[2];
	Mat magnitudes = Mat(degraded.size(), CV_32FC1);
	split(filterSpectrum,pl);
	magnitude(pl[0], pl[1],magnitudes);

	for(int x = 0; x < degraded.cols; x++)
	{
		for(int y = 0; y < degraded.rows; y++)
		{
			double div = pow(magnitudes.at<float>(y,x),2) + 1.0/pow(snr,2);
			q.at<Vec2f>(y,x)[0] = filterSpectrum.at<Vec2f>(y,x)[0] / div;
			q.at<Vec2f>(y,x)[1] = -filterSpectrum.at<Vec2f>(y,x)[1] / div;
		}
	}

	mulSpectrums(degradedSpectrum, q, out, 0);

	dft(out, out, DFT_REAL_OUTPUT + DFT_SCALE + DFT_INVERSE);
	normalize(out, out, 0, 255, CV_MINMAX);
	return out;
}

/* *****************************
  GIVEN FUNCTIONS
***************************** */

// function calls processing function
/*
in                   :  input image
restorationType     :  integer defining which restoration function is used
kernel               :  kernel used during restoration
snr                  :  signal-to-noise ratio (only used by wieder filter)
return               :  restorated image
*/
Mat Dip4::run(const Mat& in, string restorationType, const Mat& kernel, double snr){

   if (restorationType.compare("wiener")==0){
      return wienerFilter(in, kernel, snr);
   }else{
      return inverseFilter(in, kernel);
   }

}

// function degrades the given image with gaussian blur and additive gaussian noise
/*
img         :  input image
degradedImg :  degraded output image
filterDev   :  standard deviation of kernel for gaussian blur
snr         :  signal to noise ratio for additive gaussian noise
return      :  the used gaussian kernel
*/
Mat Dip4::degradeImage(const Mat& img, Mat& degradedImg, double filterDev, double snr){

    int kSize = round(filterDev*3)*2 - 1;
   
    Mat gaussKernel = getGaussianKernel(kSize, filterDev, CV_32FC1);
    gaussKernel = gaussKernel * gaussKernel.t();

    Mat imgs = img.clone();
    dft( imgs, imgs, CV_DXT_FORWARD, img.rows);
    Mat kernels = Mat::zeros( img.rows, img.cols, CV_32FC1);
    int dx, dy; dx = dy = (kSize-1)/2.;
    for(int i=0; i<kSize; i++) for(int j=0; j<kSize; j++) kernels.at<float>((i - dy + img.rows) % img.rows,(j - dx + img.cols) % img.cols) = gaussKernel.at<float>(i,j);
	dft( kernels, kernels, CV_DXT_FORWARD );
	mulSpectrums( imgs, kernels, imgs, 0 );
	dft( imgs, degradedImg, CV_DXT_INV_SCALE, img.rows );
	
    Mat mean, stddev;
    meanStdDev(img, mean, stddev);

    Mat noise = Mat::zeros(img.rows, img.cols, CV_32FC1);
    randn(noise, 0, stddev.at<double>(0)/snr);
    degradedImg = degradedImg + noise;
    threshold(degradedImg, degradedImg, 255, 255, CV_THRESH_TRUNC);
    threshold(degradedImg, degradedImg, 0, 0, CV_THRESH_TOZERO);

    return gaussKernel;
}

// Function displays image (after proper normalization)
/*
win   :  Window name
img   :  image that shall be displayed
cut   :  determines whether to cut or scale values outside of [0,255] range
*/
void Dip4::showImage(const char* win, const Mat& img, bool cut){

   Mat tmp = img.clone();

   if (tmp.channels() == 1){
      if (cut){
         threshold(tmp, tmp, 255, 255, CV_THRESH_TRUNC);
         threshold(tmp, tmp, 0, 0, CV_THRESH_TOZERO);
      }else
         normalize(tmp, tmp, 0, 255, CV_MINMAX);
         
      tmp.convertTo(tmp, CV_8UC1);
   }else{
      tmp.convertTo(tmp, CV_8UC3);
   }
   imshow(win, tmp);
}

// function calls basic testing routines to test individual functions for correctness
void Dip4::test(void){

   test_circShift();
   cout << "Press enter to continue"  << endl;
   cin.get();

}

void Dip4::test_circShift(void){
   
   Mat in = Mat::zeros(3,3,CV_32FC1);
   in.at<float>(0,0) = 1;
   in.at<float>(0,1) = 2;
   in.at<float>(1,0) = 3;
   in.at<float>(1,1) = 4;
   Mat ref = Mat::zeros(3,3,CV_32FC1);
   ref.at<float>(0,0) = 4;
   ref.at<float>(0,2) = 3;
   ref.at<float>(2,0) = 2;
   ref.at<float>(2,2) = 1;
   
   if (sum((circShift(in, -1, -1) == ref)).val[0]/255 != 9){
      cout << "ERROR: Dip4::circShift(): Result of circshift seems to be wrong!" << endl;
      return;
   }
   cout << "Message: Dip4::circShift() seems to be correct" << endl;
}
