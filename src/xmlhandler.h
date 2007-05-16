#ifndef GPXMLHANDLER_H
#define GPXMLHANDLER_H

#include <QtXml/QXmlDefaultHandler>
#include <QtCore/QDebug>

class PlaceContainer;
class PlaceMark;

class KAtlasXmlHandler : public QXmlDefaultHandler {
public:
	KAtlasXmlHandler();
	KAtlasXmlHandler( PlaceContainer* );

	bool startDocument();
	bool stopDocument();

	bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes& attrs );
	bool endElement( const QString&, const QString&, const QString &name );

	bool characters( const QString& str );

protected:
	PlaceContainer* m_placecontainer;
	PlaceMark* m_placemark;

	bool m_inKml;
	bool m_inPlacemark;
	bool m_inPoint;
	bool m_coordsset;

	QString m_currentText;
	inline int popIdx( int population );
};

#endif // GPXMLHANDLER_H
