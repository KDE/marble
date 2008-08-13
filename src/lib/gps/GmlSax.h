//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
//

#ifndef GMLSAX_H
#define GMLSAX_H

#include <QtCore/QString>
#include <QtXml/QXmlDefaultHandler>

class QXmlAttributes;
class QXmlParseException;

namespace Marble
{

/**
A very simple Xml Parser that will be based on the Gml Scema ( Geographic Markup Language )

	@author Andrew Manson <g.real.ate@gmail.com>
*/
class GmlSax : public QXmlDefaultHandler
{
public:
    GmlSax( double *lon, double *lat );

    ~GmlSax();

    bool startElement( const QString &namespaceURI,
                       const QString &localName,
                       const QString &qName,
                       const QXmlAttributes &attributes);
    bool endElement( const QString &namespaceURI,
                     const QString &localName,
                     const QString &qName );
    bool fatalError(const QXmlParseException &exception);

    bool characters( const QString &str );

private:
    double *m_lat;
    double *m_lon;

    QString currentText;
};

}

#endif
