#include <QtGui>
#include <QVector>

#define ENABLEGUI

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	uint level = 8;
	QImage imgsrc("../data/maps/etopo2/etopo2.jpg");
	QImage imgscl, tile, result;
/*	
	QVector<QRgb> legpal;
	for ( int count = 0; count < 255; count++){
		legpal.insert(count, qRgb(count, count, count));
	}
*/	 

	for ( uint count=1; count<=level; count*=2 ) {
		imgscl = imgsrc.scaled( imgsrc.width()/count, imgsrc.height()/count, Qt::KeepAspectRatio );
		for ( uint col=0; col<2*level/count; col++ )
		for ( uint row=0; row<level/count; row++ ) 
		{
			tile = imgscl.copy(col*imgsrc.width()/level/2, row*imgsrc.height()/level,
					imgsrc.width()/level/2, imgsrc.height()/level);
			QString path = QString("../data/maps/etopo2/etopo_%1_%2x%3.jpg" ).arg(level/count).arg(col).arg(row);
//			result=tile.convertToFormat(QImage::Format_Indexed8, legpal, Qt::ThresholdDither);
			result=tile;
			qDebug();
/*
			for (int x=0; x < result.width(); x++)
				for (int y=0; y < result.height(); y++){
					if (result.pixelIndex(x,y) < 20) {
						result.setPixel(x,y,20);
					}
					if (result.pixelIndex(x,y) == 0) {
						qDebug("Yes");
					}
				}
*/
			result.save(path, "jpg" ,95);
 			qDebug( ) << "Saving tile " << path;
		}
	}
        app.exit();
 }
