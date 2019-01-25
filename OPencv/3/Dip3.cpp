//============================================================================
// Name    : Dip3.cpp
// Author   : Ronny Haensch
// Version    : 2.0
// Copyright   : -
// Description :
//============================================================================

#include "Dip3.h"

// Generates gaussian filter kernel of given size
/*
kSize:     kernel size (used to calculate standard deviation)
return:    the generated filter kernel
*/
Mat Dip3::createGaussianKernel(int kSize){
  Mat kernel = Mat(kSize, kSize, CV_32FC1);

  int mu = kSize / 2;
  float sigma = kSize / 10.f;
  float sigma2 = sigma * sigma;

  float scaling = (2 * CV_PI * sigma2);

  for (int x = 0 ; x < kSize ; x++){
    for (int y = 0 ; y < kSize ; y++){
      kernel.at<float>(y,x) = exp(-0.5f * (pow(x - mu,2) + pow(y-mu,2)) / sigma2) / scaling;
    }
  }
  return kernel;
}


// Performes a circular shift in (dx,dy) direction
/*
in       input matrix
dx       shift in x-direction
dy       shift in y-direction
return   circular shifted matrix
*/
Mat Dip3::circShift(const Mat& in, int dx, int dy){
  Mat result = Mat(in.size(), CV_32FC1);
  for (int x = 0 ; x < in.cols ; x++){
    for (int y = 0 ; y < in.rows ; y++){
      result.at<float>(y,x) = in.at<float>((y - dy) % in.rows, (x - dx) % in.cols);
    }
  }
  return result;
}

//Performes convolution by multiplication in frequency domain
/*
in       input image
kernel   filter kernel
return   output image
*/
Mat Dip3::frequencyConvolution(const Mat& in, const Mat& kernel){
  Mat kernel_large = Mat::zeros(in.size(), CV_32FC1);
  kernel.copyTo(kernel_large(Rect(0,0,kernel.cols, kernel.rows)));
  kernel_large = circShift(kernel_large, -kernel.cols/2, -kernel.rows/2);

  Mat in_sp = Mat(in.size(), CV_32FC1);
  Mat kernel_sp = Mat(in.size(), CV_32FC1);

  dft(in, in_sp, 0);
  dft(kernel_large, kernel_sp, 0);

  Mat out = Mat(in.size(), CV_32FC1);
  mulSpectrums(in_sp, kernel_sp, out, 0);
  dft(out, out, DFT_INVERSE + DFT_SCALE);

  return out;
}

// Performs UnSharp Masking to enhance fine image structures
/*
in       the input image
type     integer defining how convolution for smoothing operation is done
         0 <==> spatial domain; 1 <==> frequency domain; 2 <==> seperable filter; 3 <==> integral image
size     size of used smoothing kernel
thresh   minimal intensity difference to perform operation
scale    scaling of edge enhancement
return   enhanced image
*/
Mat Dip3::usm(const Mat& in, int type, int size, double thresh, double scale){

  // some temporary images
  Mat tmp(in.rows, in.cols, CV_32FC1);

  // calculate edge enhancement

  // 1: smooth original image
  //    save result in tmp for subsequent usage
  if (type >= 0 && type <=3)
    tmp = mySmooth(in, size, type);
  else
    GaussianBlur(in, tmp, Size(floor(size/2)*2+1, floor(size/2)*2+1), size/5., size/5.);

  tmp = in - tmp;
  threshold(tmp, tmp, thresh, 0, THRESH_TOZERO);
  tmp = in + scale * tmp;

  threshold(tmp, tmp, 255, 0, THRESH_TRUNC); // if larger than 255, truncatre to 255
  threshold(tmp, tmp, 0, 0 , THRESH_TOZERO); // if smaller than 0, set to 0

  return tmp;
}

// convolution in spatial domain
/*
src:    input image
kernel:  filter kernel
return:  convolution result
*/
Mat Dip3::spatialConvolution(const Mat& src, const Mat& kernel){
	unsigned int kernelSize = kernel.rows;
	unsigned int imgWidth = src.cols;
	unsigned int imgHeigth = src.rows;
	Mat dst = Mat::ones(imgHeigth,imgWidth, CV_32FC1);

	for(unsigned int r = 0; r < imgHeigth; r++)
	{
		for(unsigned int c = 0; c < imgWidth; c++)
		{
			float sum = 0;
			for(unsigned int i = 0; i < kernelSize; i++)
			{
				for(unsigned int j = 0; j < kernelSize; j++)
				{
					unsigned int y = r-kernelSize/2+i;
					unsigned int x = c-kernelSize/2+j;
					if(y < 0 || y > imgHeigth-1 || x < 0 || x > imgWidth-1)
						sum += 255.0 * kernel.at<float>(kernelSize-1-i,kernelSize-1-j);
					else
						sum += src.at<float>(y,x) * kernel.at<float>(kernelSize-1-i,kernelSize-1-j);
				}
			}
			dst.at<float>(r,c) = sum;
		}
	}
   return dst.clone();
}


// convolution in spatial domain by seperable filters
/*
src:    input image
size     size of filter kernel
return:  convolution result
*/
Mat Dip3::seperableFilter(const Mat& src, int size){

   // optional

   return src;

}

// convolution in spatial domain by integral images
/*
src:    input image
size     size of filter kernel
return:  convolution result
*/
Mat Dip3::satFilter(const Mat& src, int size){

   // optional

   return src;

}

/* *****************************
  GIVEN FUNCTIONS
***************************** */

// function calls processing function
/*
in       input image
type     integer defining how convolution for smoothing operation is done
         0 <==> spatial domain; 1 <==> frequency domain
size     size of used smoothing kernel
thresh   minimal intensity difference to perform operation
scale    scaling of edge enhancement
return   enhanced image
*/
Mat Dip3::run(const Mat& in, int smoothType, int size, double thresh, double scale){

   return usm(in, smoothType, size, thresh, scale);

}


// Performes smoothing operation by convolution
/*
in       input image
size     size of filter kernel
type     how is smoothing performed?
return   smoothed image
*/
Mat Dip3::mySmooth(const Mat& in, int size, int type){

   // create filter kernel
   Mat kernel = createGaussianKernel(size);

   // perform convoltion
   switch(type){
     case 0: return spatialConvolution(in, kernel);	// 2D spatial convolution
     case 1: return frequencyConvolution(in, kernel);	// 2D convolution via multiplication in frequency domain
     case 2: return seperableFilter(in, size);	// seperable filter
     case 3: return satFilter(in, size);		// integral image
     default: return frequencyConvolution(in, kernel);
   }
}

// function calls basic testing routines to test individual functions for correctness
void Dip3::test(void){

   test_createGaussianKernel();
   test_circShift();
   test_frequencyConvolution();
   cout << "Press enter to continue"  << endl;
   cin.get();

}

void Dip3::test_createGaussianKernel(void){

   Mat k = createGaussianKernel(11);

   if ( abs(sum(k).val[0] - 1) > 0.0001){
      printf("ERROR: Dip3::createGaussianKernel(): Sum of all kernel elements is not one but %f!\n", abs(sum(k).val[0]));
      return;
   }
   if (sum(k >= k.at<float>(5,5)).val[0]/255 != 1){
      cout << "ERROR: Dip3::createGaussianKernel(): Seems like kernel is not centered!" << endl;
      return;
   }
   cout << "Message: Dip3::createGaussianKernel() seems to be correct" << endl;
}

void Dip3::test_circShift(void){

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
      cout << "ERROR: Dip3::circShift(): Result of circshift seems to be wrong!" << endl;
      return;
   }
   cout << "Message: Dip3::circShift() seems to be correct" << endl;
}

void Dip3::test_frequencyConvolution(void){

   Mat input = Mat::ones(9,9, CV_32FC1);
   input.at<float>(4,4) = 255;
   Mat kernel = Mat(3,3, CV_32FC1, 1./9.);

   Mat output = frequencyConvolution(input, kernel);

   if ( (sum(output < 0).val[0] > 0) or (sum(output > 255).val[0] > 0) ){
      cout << "ERROR: Dip3::frequencyConvolution(): Convolution result contains too large/small values!" << endl;
      return;
   }
   float ref[9][9] = {{0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 1, 1, 1, 1, 1, 1, 1, 0},
                      {0, 1, 1, 1, 1, 1, 1, 1, 0},
                      {0, 1, 1, (8+255)/9., (8+255)/9., (8+255)/9., 1, 1, 0},
                      {0, 1, 1, (8+255)/9., (8+255)/9., (8+255)/9., 1, 1, 0},
                      {0, 1, 1, (8+255)/9., (8+255)/9., (8+255)/9., 1, 1, 0},
                      {0, 1, 1, 1, 1, 1, 1, 1, 0},
                      {0, 1, 1, 1, 1, 1, 1, 1, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0, 0}};
   for(int y=1; y<8; y++){
      for(int x=1; x<8; x++){
         if (abs(output.at<float>(y,x) - ref[y][x]) > 0.0001){
            cout << "ERROR: Dip3::frequencyConvolution(): Convolution result contains wrong values!" << endl;
            return;
         }
      }
   }
   cout << "Message: Dip3::frequencyConvolution() seems to be correct" << endl;
}
