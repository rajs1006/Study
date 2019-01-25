//============================================================================
// Name        : main.cpp
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : only calls processing and test routines
//============================================================================

#include <iostream>

#include "Dip1.h"

using namespace std;

// usage: path to image in argv[1]
// main function. loads and saves image
int main(int argc, char** argv) {

	// will contain path to input image (taken from argv[1])
	string fname;

	// check if image path was defined
	if (argc != 2){
	    cout << "Usage: dip1 <path_to_image>" << endl;
	    cout << "Press enter to continue..." << endl;
	    cin.get();
	    return -1;
	}else{
	    // if yes, assign it to variable fname
	    fname = argv[1];
	}
	
	// construct processing object
	Dip1 dip1;

	// start the processing
	dip1.run(fname);

	// run some test routines
	dip1.test(fname);

	return 0;
}
