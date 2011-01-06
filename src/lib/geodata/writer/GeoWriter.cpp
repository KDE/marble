//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoWriter.h"

#include "GeoTagWriter.h"
#include "KmlElementDictionary.h"
#include "DgmlElementDictionary.h"

#include "MarbleDebug.h"

namespace Marble
{

GeoWriter::GeoWriter()
{
    //FIXME: work out a standard way to do this.
    m_documentType = kml::kmlTag_nameSpace22;
}

bool GeoWriter::write(QIODevice* device, const GeoNode *feature)
{
    setDevice( device );
    setAutoFormatting( true );
    writeStartDocument();

    //FIXME: write the starting tags. Possibly register a tag handler to do this
    // with a null string as the object name?
    
    GeoTagWriter::QualifiedName name( "", m_documentType );
    const GeoTagWriter* writer = GeoTagWriter::recognizes(name);
    if( writer ) {
        //FIXME is this too much of a hack?
        //geodataobject is never used in this context
        GeoNode* node = new GeoNode;
        writer->write( node, *this );
    } else {
        qDebug() << "There is no GeoWriter registered for: " << name;
        return false;
    }
    
    if( ! writeElement( feature ) ) {
        return false;
    }

    //close the document
    writeEndElement();
    return true;
}

bool GeoWriter::writeElement(const GeoNode *object)
{
    // Add checks to see that everything is ok here

    GeoTagWriter::QualifiedName name( object->nodeType(), m_documentType );
    const GeoTagWriter* writer = GeoTagWriter::recognizes( name );

    if( writer ) {
        if( ! writer->write( object, *this ) ) {
            qDebug() << "An error has been reported by the GeoWriter for: "
                    << name;
            return false;
        }
    } else {
        qDebug() << "There is no GeoWriter registered for: " << name;
        return true;
    }
    return true;
}


void GeoWriter::setDocumentType( const QString &documentType )
{
    m_documentType = documentType;
}

void GeoWriter::writeElement( const QString &key, const QString &value )
{
    writeStartElement( key );
    writeCharacters( value );
    writeEndElement();
}

void GeoWriter::writeOptionalElement( const QString &key, const QString &value )
{
    if( !value.isEmpty() ) {
        writeElement( key, value );
    }
}

}
