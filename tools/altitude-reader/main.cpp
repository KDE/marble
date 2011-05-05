#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QApplication>
#include <QDebug>
#include <qimagewriter.h>
#include <math.h>

int main( int argc, char **argv )
{
    QApplication app(argc, argv);
    float lon,lat;
    lon = 13.22781801;
    lat = 47.95088865;
    //qDebug() << QImageWriter::supportedImageFormats();
    QImage image("/home/niko/kdesvn/srtm2postgis/data/Eurasia/N47E013.tif");
    qDebug() << image.isNull();
    qDebug() << image.width() << image.height();
    /*
    for (int x=0;x<image.width();++x) {
        for (int y=0;y<image.height();++y) {
//             qDebug() << x << y << QColor(image.pixel(x, y));
        }
    }
    */
/*
    dahoam:
    lon = 13.22781801;
    lat = 47.95088865;
    635
    4278848010

    sommerholz
    lon = 13.2707119;
    lat = 47.91962086;
    689
    4278716424
*/
    
    int x = (1201 * (lon-13)) / 1;
    int y = (1201 * (lat-47)) / 1;
    qDebug() << "x" << x << "y" << y;
    qDebug() << "sizeof(QRgb)" << sizeof(QRgb);
    qDebug() << image.pixel(x, y) << QColor(image.pixel(x, y));
    unsigned int datum = image.pixel(x, y);
    qDebug() << (datum>>8 | datum<<8);
    return 0;
/*
1 tile = 1° = 1201px

1° .... 1201px
.2444° .... ?

*/
    const int width = image.width();
    const int height = image.height();

    const int m_numXTiles = 360;
    const int m_numYTiles = 360;
    const int m_radius = (360*width) / (2*M_PI);


    const qreal textureX = ( 0.5 + 0.5 * lon / M_PI ) * m_numXTiles * width;
    const qreal textureY = ( 0.5 - lat / M_PI ) * m_numYTiles * height;
    qDebug() << "textureX" << textureX << "textureY" << textureY;

    qreal color = 0;

    for ( int i = 0; i < 4; ++i ) {
        const int x = ( static_cast<int>( textureX ) + ( i % 2 ) ) % ( m_numXTiles * width );
        const int y = ( static_cast<int>( textureY ) + ( i / 2 ) ) % ( m_numYTiles * height );

        const qreal dx = ( textureX > (qreal)x ) ? textureX - (qreal)x : (qreal)x - textureX;
        const qreal dy = ( textureY > (qreal)y ) ? textureY - (qreal)y : (qreal)y - textureY;
        color += image.pixel( x % width, y % height ) * (1- dx) * (1-dy);
    }

    qDebug() << color * m_radius / 6378137.0 / 1000;









    QLabel l;
    l.setPixmap(QPixmap::fromImage(image));
    l.show();

    return app.exec();
}
