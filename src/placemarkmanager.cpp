#include "placemarkmanager.h"

#include <QDebug>

#include <QDataStream>
#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include "xmlhandler.h"

PlaceMarkManager::PlaceMarkManager(){
	m_placecontainer = new PlaceContainer(); 

	queryStatus();
}

void PlaceMarkManager::queryStatus(){

	QString defaultcachename = KAtlasDirs::path( "placemarks/stdplacemarks.cache");
	QString defaultsrcname = KAtlasDirs::path( "placemarks/stdplacemarks.kml");

	if ( !QFile::exists( defaultcachename ) ){

		qDebug("No Default Placemark Cache File!");

		if ( QFile::exists( defaultsrcname ) ){
			importKml( defaultsrcname, m_placecontainer );
			saveFile( KAtlasDirs::localDir() + "/placemarks/stdplacemarks.cache", m_placecontainer );
		}
		else {
			qDebug("No Default Placemark Source File!");
		}
	}
	else {
		qDebug("Loading Default Placemark Cache File!");
		loadFile( KAtlasDirs::path( "/placemarks/stdplacemarks.cache" ), m_placecontainer );
	}

}

void PlaceMarkManager::loadKml( QString filename ){ 

// this still is buggy and needs a lot of work as does the concept as a whole ...

//	PlaceContainer* tmp = m_placecontainer;
	m_placecontainer -> clear();
//	tmp -> deleteAll();
//	delete tmp;

	importKml( filename, m_placecontainer );
}

void PlaceMarkManager::importKml( QString filename, PlaceContainer* placecontainer ){ 

	KAtlasXmlHandler handler( placecontainer );

	QFile file( filename );

	// gzip reader:
/*
	QDataStream dataIn(&file);
	QByteArray compByteArray;
	dataIn >> compByteArray;
	QByteArray xmlByteArray = qUncompress(compByteArray);
	QString xmlString = QString::fromUtf8(xmlByteArray.data(), xmlByteArray.size());
	QXmlInputSource source;
	source.setData(xmlString);
*/
	QXmlInputSource source( &file );

	QXmlSimpleReader reader;
	reader.setContentHandler( &handler );
	reader.parse( source );
}

void PlaceMarkManager::saveFile( QString filename, PlaceContainer* placecontainer ){ 

	if ( QDir( KAtlasDirs::localDir() + "/placemarks/" ).exists() == false ) 
		( QDir::root() ).mkpath( KAtlasDirs::localDir() + "/placemarks/" );
	
	QFile file( filename );
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);

	// Write a header with a "magic number" and a version
//	out << (quint32)0xA0B0C0D0;
	out << (quint32)0x31415926;
	out << (qint32)001;

	out.setVersion(QDataStream::Qt_4_0);

	float lng, lat;

	PlaceContainer::const_iterator it;

	for ( it=placecontainer->constBegin(); it != placecontainer->constEnd(); it++ ){ // STL iterators
		out << (*it) -> name();
		(*it) -> coordinate(lng, lat);
		out << lng << lat;
		out << QString( (*it) -> role() );
		out << (qint32)(*it) -> symbol();
		out << (qint32)(*it) -> population();
	}

}

void PlaceMarkManager::loadFile( QString filename, PlaceContainer* placecontainer ){ 
	QFile file( filename );
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);

	// Read and check the header
	quint32 magic;
	in >> magic;
	if (magic != 0x31415926) {
		qDebug( "Bad file format!" );
		return;
	}
	// Read the version
	qint32 version;
	in >> version;
	if (version < 001) {
		qDebug( "Bad file - too old!" );
		return;
	} 
/*
	if (version > 002) {
		qDebug( "Bad file - too new!" );
		return;
	} 
*/

	in.setVersion(QDataStream::Qt_4_0);

// Read the data
	float lng, lat;
	QString tmpstr;
	qint32 a;

	PlaceMark* mark;

	while (	!in.atEnd() ){
		mark = new PlaceMark();
		
		in >> tmpstr;
		mark -> setName( tmpstr );
		in >> lng >> lat;
		mark -> setCoordinate(lng, lat);
		in >> tmpstr;
		mark -> setRole( tmpstr.at(0) );
		in >> a;
		mark -> setSymbol( a );
		in >> a;
		mark -> setPopulation( a );

		placecontainer -> append( mark );
	}
}
