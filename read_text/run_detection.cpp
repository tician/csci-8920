#include "text_detect.h"
#include <iostream>
#include <cstdio>

// Our window title/name (can be anything you want, obviously)
const char* WIN_DOH = "Yum, donuts";
const char* WIN_DER = "Holy macaroni";

int main(int argc, char* argv[])
{
	DetectText detector = DetectText();
	detector.readLetterCorrelation(argv[1]);
	detector.readWordList(argv[2]);


	// Use any available capture source/camera
	VideoCapture capitan( 1 );//CV_CAP_ANY );

	// Check that the capture source was successfully opened
	if ( !capitan.isOpened() )
	{
		cout << "ERROR: capture is NULL \n";
		getchar();
		exit(-1);
	}

	// Get size of capture source's image output (usually 640x480)
	Size capitanSz = Size(	(int) capitan.get(CV_CAP_PROP_FRAME_WIDTH),
							(int) capitan.get(CV_CAP_PROP_FRAME_HEIGHT));
	// Create our first window
	namedWindow( WIN_DOH );//, CV_WINDOW_AUTOSIZE );
	// Create our second window
	namedWindow( WIN_DER );//, CV_WINDOW_AUTOSIZE );
	// Move the first window to top left corner of screen
	cvMoveWindow(WIN_DOH, 0, 20);
	// Move the second window to right side of first window
	cvMoveWindow(WIN_DER, capitanSz.width, 20);

	Mat detect_frame;
	// Start our infinite loop
	while (1)
	{
		// Create empty Mat for storage of the captured image frame
		Mat capture_frame;
		Mat gray_frame;

		// Capture an image frame from the source
		capitan >> capture_frame;
		// Check that the capture source actually gave us image data
		if (capture_frame.empty())
		{
			cout << "ERROR: frame is null...\n";
			getchar();
			break;
		}

		// Convert the raw color image to a grayscale image
		cvtColor( capture_frame, gray_frame, CV_BGR2GRAY );
		// Down-scale the grayscale image by 2 (640x480 -> 320x240)
//		pyrDown( gray_frame, gray_frame, Size( gray_frame.cols/2, gray_frame.rows/2) );
		// Show the grayscale and downsized image in the first auto-sized window
		imshow( WIN_DOH, gray_frame );

		char temp = waitKey(10);

		// Check what key was pressed
		// if temp==-1, then waitKey() timed out (there was no keypress)
		if (temp>=0)
		{
			// If it was a 'f', then save the frame to a file in the local directory
			if (temp == 'f')
			{
				imwrite("./frame_grab.jpg", capture_frame);
				detector.detect("./frame_grab.jpg");
				detect_frame = imread("./frame_grab_detection.jpg");
				// Show the captured image frame in the auto-sized window
				imshow( WIN_DER, detect_frame );
			}
			// Otherwise, break the loop and exit the program
			else if (temp == 'q')
			{
				break;
			}
		}
	}



/*
	if (argc < 4)
	{
		cout << "not enought input: detect_text <image> <correlation> <dictionary>"
		<< endl;
		return -1;
	}
	DetectText detector = DetectText();
	detector.readLetterCorrelation(argv[2]);
	detector.readWordList(argv[3]);
	detector.detect(string(argv[1]));
*/
	return 0;
}
