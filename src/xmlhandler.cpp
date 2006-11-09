#include "xmlhandler.h"

#include <cmath>
#include "placecontainer.h"
#include "placemark.h"

const float deg2rad = M_PI/180.0f;

KAtlasXmlHandler::KAtlasXmlHandler(){
	m_placecontainer = new PlaceContainer("placecontainer");
	m_placemark = 0;
	m_coordsset = false;
}

KAtlasXmlHandler::KAtlasXmlHandler( PlaceContainer* placecontainer ){
	m_placecontainer = placecontainer;
	m_placemark = 0;
	m_coordsset = false;
}


bool KAtlasXmlHandler::startDocument(){ 

	m_inKml = false;
	m_inPlacemark = false;
	m_inPoint = false;

	qDebug("Starting KML-Import"); 

	return true;
}


bool KAtlasXmlHandler::startElement( const QString&, const QString&, const QString &name, const QXmlAttributes& attrs ){

	QString nameLower = name.toLower();

	if ( nameLower == "kml" ){
		m_inKml = true;
	}

	if ( m_inKml && nameLower == "placemark"){
		m_inPlacemark = true;
		m_coordsset = false;
		m_placemark = new PlaceMark();
		m_placemark->setSymbol( 0 );
	} 

	if ( m_inPlacemark && nameLower == "name"){
		m_currentText="";
	}

	if ( m_inPlacemark && nameLower == "description"){
		m_currentText="";
	}

	if ( m_inPlacemark && nameLower == "pop"){
		m_currentText="";
	}

	if ( m_inPlacemark && nameLower == "point"){
		m_inPoint = true;
	} 

	if ( m_inPlacemark && nameLower == "role"){
		m_currentText="";
	}

	if ( m_inPoint && nameLower == "coordinates"){
		m_currentText="";
	}

	return true;
}

bool KAtlasXmlHandler::characters( const QString& str ){
	m_currentText += str;
	return true;
}

bool KAtlasXmlHandler::endElement( const QString&, const QString&, const QString &name ){

	QString nameLower = name.toLower();

	if ( nameLower == "kml"){
		m_inKml = false;
	}

	if ( m_inKml && nameLower == "placemark"){

		if ( m_placemark->role() == 'P' ) m_placemark->setSymbol(16);
		else if ( m_placemark->role() == 'M' ) m_placemark->setSymbol(17);
		else if ( m_placemark->role() == 'H' ) m_placemark->setSymbol(18);
		else if ( m_placemark->role() == 'V' ) m_placemark->setSymbol(19);
		else m_placemark->setSymbol( m_placemark->popidx() );

		if (m_coordsset == true ) m_placecontainer->append( m_placemark );

		m_inPlacemark = false;
	}

	if ( m_inPlacemark && nameLower == "name"){
//		qDebug() << m_currentText;
		m_placemark->setName( m_currentText );
	}

	if ( m_inPlacemark && nameLower == "description"){
//		qDebug() << m_currentText;
	}

	if ( m_inPlacemark && nameLower == "pop"){
		int population = m_currentText.toInt();
		m_placemark->setPopulation( population );

		int popidx = 15;

		if(population < 2500) popidx=1;
		else if(population < 5000) popidx=2;
		else if(population < 7500) popidx=3;
		else if(population < 10000) popidx=4;
		else if(population < 25000) popidx=5;
		else if(population < 50000) popidx=6;
		else if(population < 75000) popidx=7;
		else if(population < 100000) popidx=8;
		else if(population < 250000) popidx=9;
		else if(population < 500000) popidx=10;
		else if(population < 750000) popidx=11;
		else if(population < 1000000) popidx=12;
		else if(population < 2500000) popidx=13;
		else if(population < 5000000) popidx=14;
//		else if(population < 7500000) popidx=15;

		m_placemark->setPopidx( popidx );
	}

	if ( m_inKml && nameLower == "point"){
		m_inPoint = false;
	}

	if ( m_inKml && nameLower == "role"){
		m_placemark->setRole( m_currentText.at(0) );
	}

	if ( m_inPoint && nameLower == "coordinates"){
		QStringList splitline = m_currentText.split(",");
		m_placemark->setCoordinate( deg2rad * splitline[0].toFloat(), - deg2rad * splitline[1].toFloat() );
		m_coordsset = true;
//		qDebug() << splitline[0];
	}

	return true;
}

bool KAtlasXmlHandler::stopDocument(){ 

	qDebug() << "Placemarks: " << m_placecontainer->size(); 
	return true;
}

