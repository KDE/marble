#include <QDebug>
#include <QFile>
#include <QStringList>
#include "placelist.h"
#include "katlasdirs.h"
#include "placemark.h"

PlaceList::PlaceList(){
	load(KAtlasDirs::citiesDir()+"/cities.txt");
}

void PlaceList::load(QString filename){
	QFile file( filename );
        file.open( QIODevice::ReadOnly );
        QTextStream stream( &file );  // read the data serialized from the file
//        stream.setByteOrder( QDataStream::LittleEndian );
//	short header, lat, lng;

// Iterator that points to current PolyLine in PntMap
//	QList<PntPolyLine*>::iterator it = begin();
//	int count = 0;
	QString rawline;
	QString name, state, country, capital, popstring, latstring, lngstring; 
	float population, lat, lng;

	while(!stream.atEnd()){

		rawline=stream.readLine();
		name = rawline.section("\t",0,0);
		state = rawline.section("\t",1,1);
		country = rawline.section("\t",2,2);
		capital = rawline.section("\t",3,3);
		popstring = rawline.section("\t",4,4);
		latstring = rawline.section("\t",5,5);
		lngstring = rawline.section("\t",6,6);

//		stream >> name >> state >> country >> capital >> popstring >> latstring >> lngstring;
		population = 1000*popstring.toFloat();

//			qDebug() << rawline << endl;
//			qDebug() << latstring;
//			qDebug() << lngstring;


		lng = lngstring.left(lngstring.size()-2).toFloat();
		if (lngstring.contains("E")) lng=-lng;

		lat = latstring.left(latstring.size()-2).toFloat();
		if (latstring.contains("S")) lat=-lat;

		PlaceMark *city = new PlaceMark(name,lat,lng);
		append( city );

/*
		if (name.contains("Berlin"))
		{
			qDebug() << rawline;
			qDebug() << name;
			qDebug() << state;
			qDebug() << country;
			qDebug() << capital;
			qDebug() << population;
			qDebug() << lat;
			qDebug() << lng;
		}
*/

	}
}

QStringList PlaceList::getPlaceMarkList(){
	QStringList placemarklist;

	QVector<PlaceMark*>::iterator itPlaceMark;
	const QVector<PlaceMark*>::iterator itEndPlaceMark = end();

	for ( itPlaceMark = begin(); itPlaceMark != itEndPlaceMark; ++itPlaceMark ){
		placemarklist << (*itPlaceMark)->getName();
	}

	return placemarklist;
}
