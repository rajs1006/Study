//============================================================================
// Name        : Dip1.cpp
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : 
//============================================================================

#include "Dip1.h"


/// Global Variables
const int alpha_slider_max = 300;
int alpha_slider = 0;
double alpha;
double beta;
const int beta_slider_max = 254 - 200;
int beta_slider = 0;
Mat src;
Mat dst;

/*
 * callback for trackbar
 */
void on_trackbarDigitization( int, void* )
{
 unsigned int pixe_block = alpha_slider + 1;
 if(alpha_slider <= 1)
 {
	 src.copyTo(dst);
	 return;
 }

 for(unsigned int i = 0; i < src.cols; i+=pixe_block)
 {
	 for(unsigned int j = 0; j < src.rows; j+=pixe_block)
	 {
		 unsigned int average = 0;
		 unsigned int amountPx = 0;
		 for(unsigned int a = i; a < pixe_block+i; a++)
		 {
			 for(unsigned int b = j; b < pixe_block+j; b++)
			 {
				 if(a < src.cols && b < src.rows){

					 average += (unsigned int)src.at<uchar>(b,a);
					 amountPx++;
				 }
			 }
		 }
		 average /= amountPx;

		 // set values
		 for(unsigned int a = i; a < pixe_block+i; a++)
		 {
			 for(unsigned int b = j; b < pixe_block+j; b++)
			 {
				 if(a < src.cols && b < src.rows)
					 dst.at<uchar>(b,a) = (unsigned char)average;
			 }
		 }
	 }
 }

 imshow("Result", dst );
}

/*
 * callback for trackbar
 */
void on_trackbarQuantization( int, void* )
{
	if(beta_slider == 0){
		src.copyTo(dst);
		return;
	}

	unsigned int buckets = 256 - (beta_slider + 200);
	float offset = 256.0 / (float)(buckets - 1);

	for(unsigned int i = 0; i < src.cols; i++)
	{
		for(unsigned int j = 0; j < src.rows; j++)
		{

			unsigned int px = (unsigned int)src.at<uchar>(j,i);
			for(unsigned b = 0; b < buckets-1; b++)
			{
				if(px < (offset/2+offset*b)){
					dst.at<uchar>(j,i) = (unsigned char)(b * offset);
					break;
				}
				dst.at<uchar>(j,i) = (unsigned char)(255);
			}
		}
	}
	imshow("Result", dst );
}


// function that performs some kind of (simple) image processing
/*
img	input image
return	output image
*/
Mat Dip1::doSomethingThatMyTutorIsGonnaLike(const Mat& img){
	 /// Initialize values
	alpha_slider = 0;
	cvtColor(img, src, COLOR_RGB2GRAY);
	cvtColor(img, dst, COLOR_RGB2GRAY);

	return dst;


}

/* *****************************
  GIVEN FUNCTIONS
***************************** */

// function loads input image, calls processing function, and saves result
/*
fname	path to input image
*/
void Dip1::run(string fname){

	// window names
	string win1 = string ("Original image");
	string win2 = string ("Result");
  
	// some images
	Mat inputImage, outputImage;
  
	// load image
	cout << "load image" << endl;
	inputImage = imread( fname );
	cout << "done" << endl;
	
	// check if image can be loaded
	if (!inputImage.data){
	    cout << "ERROR: Cannot read file " << fname << endl;
	    cout << "Press enter to continue..." << endl;
	    cin.get();
	    exit(-1);
	}

	// show input image
	namedWindow( win1.c_str() );
	imshow( win1.c_str(), inputImage );
	
	// do something (reasonable!)
	outputImage = doSomethingThatMyTutorIsGonnaLike( inputImage );
	
	// show result
	namedWindow( win2.c_str() );

	createTrackbar( "Digitization", "Result", &alpha_slider, alpha_slider_max, on_trackbarDigitization );
	on_trackbarDigitization( alpha_slider, 0 );

	createTrackbar( "Quantization", "Result", &beta_slider, beta_slider_max, on_trackbarQuantization );
	on_trackbarQuantization( beta_slider, 0 );

	imshow( win2.c_str(), outputImage );

	// save result
	imwrite("result.jpg", outputImage);
	
	// wait a bit
	waitKey(0);

}

// function loads input image and calls the processing functions
// output is tested on "correctness" 
/*
fname	path to the input image
*/
void Dip1::test(string fname){

	// some image variables
	Mat inputImage, outputImage;
  
	// load image
	inputImage = imread( fname );

	// check if image can be loaded
	if (!inputImage.data){
	    cout << "ERROR: Cannot read file " << fname << endl;
	    cout << "Continue with pressing enter..." << endl;
	    cin.get();
	    exit(-1);
	}



	// create output
	outputImage = doSomethingThatMyTutorIsGonnaLike( inputImage );
	// test output
	test_doSomethingThatMyTutorIsGonnaLike(inputImage, outputImage);

}

// function loads input image and calls processing function
// output is tested on "correctness" 
/*
inputImage	input image as used by doSomethingThatMyTutorIsGonnaLike(..)
outputImage	output image as created by doSomethingThatMyTutorIsGonnaLike(..)
*/
void Dip1::test_doSomethingThatMyTutorIsGonnaLike(const Mat& inputImage, const Mat& outputImage){

 	vector<Mat> inputPlanes, outputPlanes;

	// ensure that input and output have equal number of channels
	// split (multi-channel) image into planes
	if ( (inputImage.channels() == 3) and (outputImage.channels() == 1) ){
      Mat input_tmp;
		cvtColor(inputImage, input_tmp, CV_BGR2GRAY);
   	split( input_tmp, inputPlanes );
   }else{
     	split( inputImage, inputPlanes );
   }
	split( outputImage, outputPlanes );

	// number of planes (1=grayscale, 3=color)
	int numOfPlanes = inputPlanes.size();

	// calculate and compare image histograms for each plane
	Mat inputHist, outputHist;
	// number of bins
	int histSize = 100;
	float range[] = { 0, 256 } ;
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;
	double sim = 0;
	for(int p = 0; p < numOfPlanes; p++){
		// calculate histogram
		calcHist( &inputPlanes[p], 1, 0, Mat(), inputHist, 1, &histSize, &histRange, uniform, accumulate );
		calcHist( &outputPlanes[p], 1, 0, Mat(), outputHist, 1, &histSize, &histRange, uniform, accumulate );
		// normalize
		inputHist = inputHist / sum(inputHist).val[0];
		outputHist = outputHist / sum(outputHist).val[0];
		// similarity as histogram intersection
		sim += compareHist(inputHist, outputHist, CV_COMP_INTERSECT);
	}
	sim /= numOfPlanes;

	// check whether images are to similar after transformation
	if (sim >= 0.8)
		cout << "Warning: The input and output image seem to be quite similar (similarity = " << sim << " ). Are you sure your tutor is gonna like your work?" << endl;

}
