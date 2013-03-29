#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <zbar.h>

using namespace cv;
using namespace std;
using namespace zbar;

const char* WIN_DOH = "Yum, donuts";
const char* WIN_DER = "Holy macaroni";

const char* keys =
{
	"{cal            |      | video calibration file}"
	"{vid            |0     | video source          }"
};

int main (int argc, const char **argv)
{
	CommandLineParser parser(argc, argv, keys);

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


	ImageScanner scanner;
	scanner.set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);


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

		int channels = gray_.channels();
		int nRows = gray_.rows;
		int nCols = gray_.cols * channels;

		uchar* raw = gray_.data;
		Image image(nCols, nRows, "Y800", raw, nCols * nRows);
		int n = scanner.scan(image);

		if (n>0)
		{
			// extract results
			for(Image::SymbolIterator symbol = image.symbol_begin();
				symbol != image.symbol_end();
				++symbol)
			{
				// do something useful with results
				cout << "decoded " << symbol->get_type_name() << " symbol "
					<< "\"" << symbol->get_data() << "\"\n";

				Point pt0, pta, ptb;
				int iter;
				int x_loc=0, y_loc=0;
				for (iter=0; iter<symbol->get_location_size(); iter++)
				{
					ptb.x = symbol->get_location_x(iter);
					ptb.y = symbol->get_location_y(iter);
					x_loc += symbol->get_location_x(iter);
					y_loc += symbol->get_location_y(iter);
		//			cout << "(" << symbol->get_location_x(iter)
		//				<< "," << symbol->get_location_y(iter) << ")\n";

					if (iter>0)
					{
						line( gray_, pta, ptb, Scalar(128,0,0), 2, 8 );
						pta.x = ptb.x;
						pta.y = ptb.y;
					}
					else
					{
						pt0.x = symbol->get_location_x(iter);
						pt0.y = symbol->get_location_y(iter);
						pta.x = pt0.x;
						pta.y = pt0.y;
					}

				}
				line( gray_, ptb, pt0, Scalar(128,0,0), 2, 8 );
				x_loc /= iter;
				y_loc /= iter;
				cout << "(" << x_loc << "," << y_loc << ")\n";

			}
		}
		else
		{
			cout << "nothing found\n";
		}

		imshow( WIN_DOH, gray_ );

		// clean up
		image.set_data(NULL, 0);

		if(waitKey(30) >= 0)
			break;
	}

	return(0);
}
