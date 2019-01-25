//============================================================================
// Name        : Dip1.h
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : header file for first DIP assignment
//============================================================================

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Dip1{

	public:
		// constructor
		Dip1(void){};
		// destructor
		~Dip1(void){};
		
		// processing routine
		void run(string);
		// testing routine
		void test(string);

	private:
		// function that performs some kind of (simple) image processing
		// --> please edit ONLY these functions!
		Mat doSomethingThatMyTutorIsGonnaLike(const Mat&);

		// test function
		void test_doSomethingThatMyTutorIsGonnaLike(const Mat&, const Mat&);
};
