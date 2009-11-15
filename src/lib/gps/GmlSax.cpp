//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
//

#include "GmlSax.h"

#include "MarbleDebug.h"
#include <QtGui/QMessageBox>

using namespace Marble;

GmlSax::GmlSax( qreal *lon, qreal *lat )
{
    m_lon = lon;
    m_lat = lat;
}


GmlSax::~GmlSax()
{
}

bool GmlSax::startElement( const QString &namespaceURI,
                           const QString &localName,
                           const QString &qName,
                           const QXmlAttributes &attributes)
{
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );
    Q_UNUSED( qName );
    Q_UNUSED( attributes );

    
    return true;
}

bool GmlSax::endElement( const QString &namespaceURI,
                         const QString &localName,
                         const QString &qName )
{
    
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );
    
    if ( qName == "gml:coordinates") {
        QString temp = currentText;
        mDebug() << temp;
        
        QStringList tempList = temp.split( ',' );
        
        mDebug() << tempList[0]<<  tempList[1];
        
        *m_lon = tempList[0].toDouble();
        *m_lat = tempList[1].toDouble();
    }

    return true;
}

bool GmlSax::fatalError(const QXmlParseException &exception)
{
    QMessageBox::warning(0, QObject::tr("GpxSax"),
                         QObject::tr("Parse error at line %1, column "
                                 "%2:\n%3.")
                                 .arg(exception.lineNumber())
                                 .arg(exception.columnNumber())
                                 .arg(exception.message()));
    return false;
}

bool GmlSax::characters( const QString &str )
{
    currentText = str;
    // TODO: can this ever be false? Tim
    return true;
}


