//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Daniel Marth <danielmarth@gmx.at>
//

#include "Activity.h"

namespace Marble
{

namespace Declarative
{

Activity::Activity( const QString& name, const QString& imagePath,
                    const QString &path ) :
    m_name ( name ),
    m_imagePath( imagePath ),
    m_path( path )
{
}

void Activity::setName( const QString& name )
{
    m_name = name;
}

QString Activity::name() const
{
    return m_name;
}

void Activity::setImagePath( const QString& imagePath )
{
    m_imagePath = imagePath;
}

QString Activity::imagePath() const
{
    return m_imagePath;
}

void Activity::setPath( const QString& path )
{
    m_path = path;
}

QString Activity::path() const
{
    return m_path;
}

}

}

#include "Activity.moc"
