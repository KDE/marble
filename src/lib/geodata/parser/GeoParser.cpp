/*
    Copyright (C) 2008 Nikolas Zimmermann <zimmermann@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/


// Own
#include "GeoParser.h"

// Marble
#include "MarbleDebug.h"

// Geodata
#include "GeoDocument.h"
#include "GeoTagHandler.h"

namespace Marble
{

// Set to a value greater than 0, to dump parent node chain while parsing
#define DUMP_PARENT_STACK 0

GeoParser::GeoParser( GeoDataGenericSourceType source )
    : QXmlStreamReader(),
      m_document( 0 ),
      m_itemModel( 0 ),
      m_source( source )
{
}

GeoParser::~GeoParser()
{
    delete m_document;
}

#if DUMP_PARENT_STACK > 0
static void dumpParentStack( const QString& name, int size, bool close )
{
    static int depth = 0;

    if ( !close )
        depth++;

    QString result;
    for ( int i = 0; i < depth; ++i )
        result += ' ';

    if ( close ) {
        depth--;
        result += "</";
    } else
        result += '<';

    result += name + "> stack size " + QString::number( size );
    fprintf( stderr, "%s\n", qPrintable( result ));
}
#endif

bool GeoParser::read( QIODevice* device )
{
    // Assert previous document got released.
    Q_ASSERT( !m_document );
    m_document = createDocument();
    Q_ASSERT( m_document );

    // Set data source
    setDevice( device );

    // Start parsing
    while ( !atEnd() ) {
        readNext();

        if ( isStartElement() ) {
            if ( isValidRootElement() ) {
#if DUMP_PARENT_STACK > 0
                dumpParentStack( name().toString(), m_nodeStack.size(), false );
#endif
 
                parseDocument();

                if ( !m_nodeStack.isEmpty() )
                    raiseError(
                        QObject::tr("Parsing failed. Still %n unclosed tag(s) after document end.", "",
                        m_nodeStack.size() ) + errorString());
            } else
                raiseRootElementError();
        }
    }

    if ( error() ) {
        mDebug() << "[GeoParser::read] -> Error occurred:" << errorString() << " at line: " << lineNumber();

        // Defer the deletion to the dtor
        // This allows the BookmarkManager to recover the broken .kml files it produced in Marble 1.0 and 1.1
        /** @todo: Remove this workaround around Marble 1.4 */
        // delete releaseDocument();
    }
    return !error();
}

bool GeoParser::isValidElement( const QString& tagName ) const
{
    return name() == tagName;
}

GeoStackItem GeoParser::parentElement( unsigned int depth )
{
    QStack<GeoStackItem>::const_iterator it = m_nodeStack.constEnd() - 1;

    if ( it - depth < m_nodeStack.constBegin() )
        return GeoStackItem();

    return *(it - depth);
}

void GeoParser::parseDocument()
{
    if( !isStartElement() ) {
        raiseError( QObject::tr("Error parsing file at line: %1 and column %2 . ")
                    .arg( lineNumber() ).arg( columnNumber() )
                    +  QObject::tr("This is an Invalid File") );
        return;
    }

    bool processChildren = true;
    GeoTagHandler::QualifiedName qName( name().toString(),
                                        namespaceUri().toString() );

    if( tokenType() == QXmlStreamReader::Invalid )
        raiseWarning( QString( "%1: %2" ).arg( error() ).arg( errorString() ) );

    GeoStackItem stackItem( qName, 0 );

    if ( const GeoTagHandler* handler = GeoTagHandler::recognizes( qName )) {
        stackItem.assignNode( handler->parse( *this ));
        processChildren = !isEndElement();
    }
    // Only add GeoStackItem to the parent chain, if the tag handler
    // for the current element possibly contains non-textual children.
    // Consider following DGML snippet "<name>Test</name>" - the
    // DGMLNameTagHandler assumes that <name> only contains textual
    // children, and reads the joined value of all children using
    // readElementText(). This implicates that tags like <name>
    // don't contain any children that would need to be processed using
    // this parseDocument() function.
    if ( processChildren ) {
        m_nodeStack.push( stackItem );
#if DUMP_PARENT_STACK > 0
        dumpParentStack( name().toString(), m_nodeStack.size(), false );
#endif
        while ( !atEnd() ) {
            readNext();
            if ( isEndElement() ) {
                m_nodeStack.pop();
#if DUMP_PARENT_STACK > 0
                dumpParentStack( name().toString(), m_nodeStack.size(), true );
#endif
                break;
            }

            if ( isStartElement() ) {
                parseDocument();
            }
        }
    }
#if DUMP_PARENT_STACK > 0
    else {
        // This is only used for debugging purposes.
        m_nodeStack.push( stackItem );
        dumpParentStack( name().toString() + "-discarded", m_nodeStack.size(), false );

        m_nodeStack.pop();
        dumpParentStack( name().toString() + "-discarded", m_nodeStack.size(), true );
    }
#endif
}

void GeoParser::raiseRootElementError()
{
    raiseError( QObject::tr( "File format unrecognized" ));
}

void GeoParser::raiseWarning( const QString& warning )
{
    // TODO: Maybe introduce a strict parsing mode where we feed the warning to
    // raiseError() (which stops parsing).
    mDebug() << "[GeoParser::raiseWarning] -> " << warning;
}

QString GeoParser::attribute( const char* attributeName ) const
{
    return attributes().value( QString::fromLatin1( attributeName )).toString();
}

GeoDocument* GeoParser::releaseDocument()
{
    GeoDocument* document = m_document;
    m_document = 0;
    return document;
}

QList<GeoGraphicsItem*>* GeoParser::activeModel()
{
    if( ! m_itemModel ) {
        m_itemModel = new QList<GeoGraphicsItem*>();
    }
    Q_ASSERT(m_itemModel);
    return m_itemModel;
}

QList<GeoGraphicsItem*>* GeoParser::releaseModel()
{
    QList<GeoGraphicsItem*>* model = m_itemModel;
    m_itemModel = 0;
    return model;
}

}
