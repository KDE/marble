//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlWhenTagHandler.h"

#include "MarbleDebug.h"
#include <QtCore/QDateTime> 

#include "KmlElementDictionary.h"
#include "GeoDataTimeStamp.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( when )

GeoNode* KmlwhenTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_when ) );

    GeoStackItem parentItem = parser.parentElement();

    if( parentItem.represents( kmlTag_TimeStamp ) ) {
        QString whenString = parser.readElementText().trimmed();
#ifdef DEBUG_TAGS
        mDebug() << "Parsed <" << kmlTag_when << "> containing: " << whenString
                 << " parent item name: " << parentItem.qualifiedName().first;
#endif // DEBUG_TAGS
	modify( whenString );
	QDateTime when = QDateTime::fromString( whenString, Qt::ISODate );
        parentItem.nodeAs<GeoDataTimeStamp>()->setWhen( when );
    }

    return 0;
}

void KmlwhenTagHandler::modify(  QString& whenString ) const
{
    switch( whenString.length() )
    {
	case 4 : whenString.append( "-01-01" );
		 break;
	case 7 : whenString.append( "-01" );
                 break;
	case 10: break;
	case 19: break;
	case 20: break;
	case 25: QString localTime = whenString.left( 19 );
		 QString sign = whenString.at( 19 );
		 bool ok;
		 QDateTime dateTime;
		 int hour = whenString.right( 5 ).left( 2 ).toInt( &ok, 10 );
		 int min = whenString.right( 2 ).toInt( &ok, 10 );
		 if( sign == "-")
		 {
		     dateTime = QDateTime::fromString( localTime, Qt::ISODate ).addSecs( hour*3600 + min*60 );
    		 }
		 else
		 {
		     dateTime = QDateTime::fromString( localTime, Qt::ISODate ).addSecs( -(hour*3600 + min*60) );
		 }
		 whenString = dateTime.toString( Qt::ISODate );
		 break;
    }

}

}

}
