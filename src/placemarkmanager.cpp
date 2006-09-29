#include "placemarkmanager.h"

#include <QDebug>

PlaceMarkManager::PlaceMarkManager(){
	queryStatus();
}

void PlaceMarkManager::queryStatus(){
	QString sourcefilename = KAtlasDirs::path( "placemarks/stdplacemarks.kml");
	if ( !QFile::exists( sourcefilename ) ){
		qDebug("No Placemark Source File!");
		return;		
	}
	QString indexfilename = KAtlasDirs::path( "placemarks/placemarkindex.dat");
	if ( !QFile::exists( indexfilename ) ){
		qDebug("No Placemark Index File!");
	}
}
