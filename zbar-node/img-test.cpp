#include <iostream>
#include <Magick++.h>
#include <zbar.h>

using namespace std;
using namespace zbar;

int main (int argc, char **argv)
{
	if(argc < 2) return(1);

	// create a reader
	ImageScanner scanner;

	// configure the reader
	scanner.set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);

	// obtain image data
	Magick::Image magick(argv[1]);	// read an image file
	int width = magick.columns();	// extract dimensions
	int height = magick.rows();
	Magick::Blob blob;				// extract the raw data
	magick.modifyImage();
	magick.write(&blob, "GRAY", 8);
	const void *raw = blob.data();

	// wrap image data
	Image image(width, height, "Y800", raw, width * height);

	// scan the image for barcodes
	int n = scanner.scan(image);

	// extract results
	for(Image::SymbolIterator symbol = image.symbol_begin();
		symbol != image.symbol_end();
		++symbol)
	{
		// do something useful with results
		cout << "decoded " << symbol->get_type_name() << " symbol "
			<< "\"" << symbol->get_data() << "\"\n";
		int iter;
		int x_loc=0, y_loc=0;
		for (iter=0; iter<symbol->get_location_size(); iter++)
		{
			x_loc += symbol->get_location_x(iter);
			y_loc += symbol->get_location_y(iter);
//			cout << "(" << symbol->get_location_x(iter)
//				<< "," << symbol->get_location_y(iter) << ")\n";
		}
		x_loc /= iter;
		y_loc /= iter;
		cout << "(" << x_loc << "," << y_loc << ")\n";
	}

	// clean up
	image.set_data(NULL, 0);

	return(0);
}
