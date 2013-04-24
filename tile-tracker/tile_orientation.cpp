#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

const char* WIN_DOH = "Yum, donuts";
const char* WIN_DER = "Holy macaroni";

/*
// For v2.4.9+ and not v2.3.1 or v2.4.4
const std::string keys =
    "{help h usage ? |      | print this message   }"
    "{@image1        |      | image1 for compare   }"
    "{@image2        |      | image2 for compare   }"
    "{@repeat        |1     | number               }"
    "{path           |.     | path to file         }"
    "{fps            | -1.0 | fps for output video }"
    "{N count        |100   | count of objects     }"
    "{ts timestamp   |      | use time stamp       }"
    ;

	CommandLineParser parser(argc, argv, keys);
	parser.about("Application name v1.0.0");

	if (parser.has("help"))
	{
		parser.printMessage();
		return 0;
	}

	int N = parser.get<int>("N");
	double fps = parser.get<double>("fps");
	std::string path = parser.get<std::string>("path");

	use_time_stamp = parser.has("timestamp");

	std::string img1 = parser.get<string>(0);
	std::string img2 = parser.get<string>(1);

	int repeat = parser.get<int>(2);

	if (!parser.check())
	{
		parser.printErrors();
		return 0;
	}

*/
// good for v2.4.4 and earlier
const char* keys =
{
	"{cal            |      | video calibration file}"
	"{vid            |0     | video source          }"
};

int main( int argc, const char** argv )
{
	CommandLineParser parser(argc, argv, keys);
//	parser.about("Tile Orientation v1.0.0");

	Mat cameraMatrix, distCoeffs;
	std::string inputSettingsFile = parser.get<std::string>("cal");
	if (inputSettingsFile.length()>0)
	{
		FileStorage fs(inputSettingsFile, FileStorage::READ);
		if (!fs.isOpened())
		{
			cout << "Could not open the calibration file: \"" << inputSettingsFile << "\"" << endl;
			return -1;
		}
		fs["Camera_Matrix"] >> cameraMatrix;
		fs["Distortion_Coefficients"] >> distCoeffs;
		fs.release();
	}

	int vidcap = parser.get<int>("vid");



	VideoCapture capitan(vidcap);
	if ( !capitan.isOpened() )
	{
		cout << "ERROR: capture is NULL \n";
		waitKey(0);
		return -1;
	}
	Size capitanSz = Size(	(int) capitan.get(CV_CAP_PROP_FRAME_WIDTH),
							(int) capitan.get(CV_CAP_PROP_FRAME_HEIGHT));
	namedWindow( WIN_DOH, CV_WINDOW_AUTOSIZE );
	namedWindow( WIN_DER, CV_WINDOW_AUTOSIZE );
	cvMoveWindow(WIN_DOH, 0, 50);
	cvMoveWindow(WIN_DER, capitanSz.width, 50);

	Mat gray_;
/*
//	Mat elem = getStructuringElement(MORPH_RECT, Size(11,11), Point(-1,-1));
	Mat dila = getStructuringElement(MORPH_RECT, Size(11,11), Point(-1,-1));
	Mat erod = getStructuringElement(MORPH_RECT, Size(5,5), Point(-1,-1));
*/

	while (1)
	{
		Mat src, gray;
		capitan >> src;
		cvtColor( src, gray, CV_BGR2GRAY );
		if (argc == 2)
		{
			undistort(gray, gray_, cameraMatrix, distCoeffs);
		}
		else
		{
			gray_ = gray.clone();
		}

#define KERNEL_SIZE	12
		bilateralFilter ( gray_, gray, KERNEL_SIZE, KERNEL_SIZE*2, KERNEL_SIZE/2 );
//		imshow( WIN_DER, gray );
		gray_ = gray.clone();

		imshow( WIN_DOH, gray_ );


		int M = getOptimalDFTSize( gray_.rows );
		int N = getOptimalDFTSize( gray_.cols );
		Mat padded;
		copyMakeBorder(gray_, padded, 0, M - gray_.rows, 0, N - gray_.cols, BORDER_CONSTANT, Scalar::all(0));

		Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
		Mat complexImg;
		merge(planes, 2, complexImg);

		dft(complexImg, complexImg);

		// compute log(1 + sqrt(Re(DFT(img))**2 + Im(DFT(img))**2))
		split(complexImg, planes);
		magnitude(planes[0], planes[1], planes[0]);
		Mat mag = planes[0];
		mag += Scalar::all(1);
		log(mag, mag);

		// crop the spectrum, if it has an odd number of rows or columns
		mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));

		int cx = mag.cols/2;
		int cy = mag.rows/2;

		// rearrange the quadrants of Fourier image
		// so that the origin is at the image center
		Mat tmp;
		Mat q0(mag, Rect(0, 0, cx, cy));
		Mat q1(mag, Rect(cx, 0, cx, cy));
		Mat q2(mag, Rect(0, cy, cx, cy));
		Mat q3(mag, Rect(cx, cy, cx, cy));

		q0.copyTo(tmp);
		q3.copyTo(q0);
		tmp.copyTo(q3);

		q1.copyTo(tmp);
		q2.copyTo(q1);
		tmp.copyTo(q2);

		normalize(mag, mag, 0, 1, CV_MINMAX);


		imshow( WIN_DER, mag );



/*


// modification to Schroeter-DAGM-03 (not yet gotten angle and power)
		Mat src, gray, etalid, edore, gradient, angle, power;
		capitan >> src;
		cvtColor( src, gray, CV_BGR2GRAY );
		if (argc == 2)
		{
			undistort(gray, gray_, cameraMatrix, distCoeffs);
		}
		else
		{
			gray_ = gray.clone();
		}
		imshow( WIN_DOH, gray_ );

//		morphologyEx(gray_, gradient, MORPH_BLACKHAT, elem);
		dilate(gray_, etalid, dila);
		erode(gray_, edore, erod);

//		gradient = etalid - edore;
		boxFilter((etalid - edore),gradient, -1, Size(5,5));

		imshow( WIN_DER, gradient );
*/

		if(waitKey(30) >= 0)
			break;
	}
	return 0;
}
