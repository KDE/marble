#include "xmlhandler.h"

#include "placecontainer.h"
#include "placemark.h"

KAtlasXmlHandler::KAtlasXmlHandler( bool ondisc ):m_ondisc(ondisc){
	m_placecontainer = new PlaceContainer("placecontainer");
	m_placemark = 0;
	m_coordsset = false;
}

KAtlasXmlHandler::KAtlasXmlHandler( PlaceContainer* placecontainer ){
	m_ondisc = false;

	m_placecontainer = placecontainer;
	m_placemark = 0;
	m_coordsset = false;
}


bool KAtlasXmlHandler::startDocument(){ 

	m_inKml = false;
	m_inPlacemark = false;
	m_inPoint = false;

	QString m_currentText;
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
		if (m_ondisc == false)
			if (m_coordsset == true ) m_placecontainer->append( m_placemark );
		else {
			qDebug( "Saving ..." );
		}
//		qDebug() << m_rootfolder->size();
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
		int symbol = 15;

		if(population < 2500) symbol=1;
		else if(population < 5000) symbol=2;
		else if(population < 7500) symbol=3;
		else if(population < 10000) symbol=4;
		else if(population < 25000) symbol=5;
		else if(population < 50000) symbol=6;
		else if(population < 75000) symbol=7;
		else if(population < 100000) symbol=8;
		else if(population < 250000) symbol=9;
		else if(population < 500000) symbol=10;
		else if(population < 750000) symbol=11;
		else if(population < 1000000) symbol=12;
		else if(population < 2500000) symbol=13;
		else if(population < 5000000) symbol=14;
//		else if(population < 7500000) symbol=15;

		m_placemark->setSymbol( symbol );
	}

	if ( m_inKml && nameLower == "point"){
		m_inPoint = false;
	}

	if ( m_inPoint && nameLower == "coordinates"){
		QStringList splitline = m_currentText.split(",");
		m_placemark->setCoordinate( splitline[0].toFloat(), splitline[1].toFloat() );
		m_coordsset = true;
//		qDebug() << splitline[0];
	}

	return true;
}

bool KAtlasXmlHandler::stopDocument(){ 

	qDebug() << "Placemarks: " << m_placecontainer->size(); 
	return true;
}

