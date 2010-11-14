//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlBeginTagHandler.h"

#include "MarbleDebug.h"
#include <QtCore/QDateTime> 

#include "KmlElementDictionary.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( begin )

GeoNode* KmlbeginTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_begin ) );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_TimeSpan ) ) {
        QString beginString = parser.readElementText().trimmed();
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_begin << "> containing: " << beginString
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
	modify( beginString );
        QDateTime begin = QDateTime::fromString( beginString, Qt::ISODate );
        parentItem.nodeAs<GeoDataTimeSpan>()->setBegin( begin );
    }

    return 0;
}

void KmlbeginTagHandler::modify(  QString& beginString ) const
{
    switch( beginString.length() )
    {
        case 4 : beginString.append( "-01-01" );
                 break;
        case 7 : beginString.append( "-01" );
                 break;
        case 10: break;
        case 19: break;
        case 20: break;
        case 25: QString localTime = beginString.left( 19 );
                 QString sign = beginString.at( 19 );
                 bool ok;
                 QDateTime dateTime;
                 int hour = beginString.right( 5 ).left( 2 ).toInt( &ok, 10 );
                 int min = beginString.right( 2 ).toInt( &ok, 10 );
                 if( sign == "-")
                 {
                     dateTime = QDateTime::fromString( localTime, Qt::ISODate ).addSecs( hour*3600 + min*60 );
                 }
                 else
                 {
                     dateTime = QDateTime::fromString( localTime, Qt::ISODate ).addSecs( -(hour*3600 + min*60) );
                 }
                 beginString = dateTime.toString( Qt::ISODate );
                 break;
    }

}

}
}
