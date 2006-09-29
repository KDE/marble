#ifndef GPXMLHANDLER_H
#define GPXMLHANDlER_H

#include <QXmlDefaultHandler>
#include <QDebug>

class PlaceContainer;
class PlaceMark;

class KAtlasXmlHandler : public QXmlDefaultHandler {
public:
	KAtlasXmlHandler( bool ondisc = false );
	KAtlasXmlHandler( PlaceContainer* );

	bool startDocument();
	bool stopDocument();

	bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes& attrs );
	bool endElement( const QString&, const QString&, const QString &name );

	bool characters( const QString& str );

private:
	PlaceContainer* m_placecontainer;
	PlaceMark* m_placemark;

	bool m_ondisc;

	bool m_inKml;
	bool m_inPlacemark;
	bool m_inPoint;
	bool m_coordsset;

	QString m_currentText;
};

#endif // GPXMLHANDLER_H
