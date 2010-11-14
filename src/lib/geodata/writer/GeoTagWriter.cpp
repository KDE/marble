//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
// Copyright (C) 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
//

#include "GeoTagWriter.h"

#include "GeoWriter.h"

namespace Marble
{

GeoTagWriter::TagHash* GeoTagWriter::s_tagWriterHash = 0;

GeoTagWriter::GeoTagWriter()
{
}

GeoTagWriter::~GeoTagWriter()
{
}

bool GeoTagWriter::writeElement( const GeoDataObject &object,
                                 GeoWriter &writer) const
{
    return writer.writeElement( object );
}

void GeoTagWriter::registerWriter(const QualifiedName& name,
                                  const GeoTagWriter* writer )
{
    TagHash* tagHash = tagWriterHash();

    Q_ASSERT( !tagHash->contains( name ) );
    tagHash->insert( name, writer );
    Q_ASSERT( tagHash->contains( name ) );
}

GeoTagWriter::TagHash* GeoTagWriter::tagWriterHash()
{
    if( !s_tagWriterHash ) {
        s_tagWriterHash = new TagHash();
    }

    Q_ASSERT( s_tagWriterHash );
    return s_tagWriterHash;
}

const GeoTagWriter* GeoTagWriter::recognizes( const QualifiedName &qname )
{
    TagHash* hash = tagWriterHash();

    if( !hash->contains( qname ) ) {
        return 0;
    }

    return hash->value( qname );
}

}
