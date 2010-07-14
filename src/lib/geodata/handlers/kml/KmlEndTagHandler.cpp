//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlEndTagHandler.h"

#include "MarbleDebug.h"
#include <QtCore/QDateTime> 

#include "KmlElementDictionary.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( end )

GeoNode* KmlendTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_end ) );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_TimeSpan ) ) {
        QString endString = parser.readElementText().trimmed();
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_end << "> containing: " << endString
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
	modify( endString );
	QDateTime end = QDateTime::fromString( endString, Qt::ISODate );
        parentItem.nodeAs<GeoDataTimeSpan>()->setEnd( end );
    }

    return 0;
}

void KmlendTagHandler::modify(  QString& endString ) const
{
    switch( endString.length() )
    {
        case 4 : endString.append( "-01-01" );
                 break;
        case 7 : endString.append( "-01" );
                 break;
        case 10: break;
        case 19: break;
        case 20: break;
        case 25: QString localTime = endString.left( 19 );
                 QString sign = endString.at( 19 );
                 bool ok;
                 QDateTime dateTime;
                 int hour = endString.right( 5 ).left( 2 ).toInt( &ok, 10 );
                 int min = endString.right( 2 ).toInt( &ok, 10 );
                 if( sign == "-")
                 {
                     dateTime = QDateTime::fromString( localTime, Qt::ISODate ).addSecs( hour*3600 + min*60 );
                 }
                 else
                 {
                     dateTime = QDateTime::fromString( localTime, Qt::ISODate ).addSecs( -(hour*3600 + min*60) );
                 }
                 endString = dateTime.toString( Qt::ISODate );
                 break;
    }

}

}
}
