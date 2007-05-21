#ifndef GPXMLHANDLER_H
#define GPXMLHANDLER_H


#include <QtCore/QDebug>
#include <QtXml/QXmlDefaultHandler>


class PlaceMark;
class PlaceContainer;


class KAtlasXmlHandler : public QXmlDefaultHandler
{
 public:
    KAtlasXmlHandler();
    KAtlasXmlHandler( PlaceContainer* );

    bool startDocument();
    bool stopDocument();

    bool startElement( const QString&, const QString&, const QString &name, 
                       const QXmlAttributes& attrs );
    bool endElement( const QString&, const QString&, const QString &name );

    bool characters( const QString& str );

 protected:
    inline int popIdx( int population );

 private:
    PlaceContainer  *m_placecontainer;
    PlaceMark       *m_placemark;

    QString          m_currentText;

    // State machine?
    bool             m_inKml;
    bool             m_inPlacemark;
    bool             m_inPoint;

    bool             m_coordsset;
};

#endif // GPXMLHANDLER_H
