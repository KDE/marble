//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Dennis Nienhüser <nienhueser@kde.org>

#include "KmlDocument.h"

#include "MarbleDebug.h"

#include <QFile>
#include <QDir>

namespace Marble
{

KmlDocument::~KmlDocument()
{
    foreach( const QString &file, m_files ) {
        if ( !QFile::remove( file ) ) {
            mDebug() << "Failed to remove temporary file" << file;
        }
    }
    if ( !m_path.isEmpty() ) {
        removeDirectoryRecursively( m_path );
    }
}

void KmlDocument::setFiles( const QString &path, const QStringList &files )
{
    m_path = path;
    m_files = files;
}

void KmlDocument::removeDirectoryRecursively( const QString &path )
{
    QStringList const subdirs = QDir( path ).entryList( QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot );
    foreach( const QString &subdir, subdirs ) {
        removeDirectoryRecursively(path + QLatin1Char('/') + subdir);
    }
    QDir::root().rmdir( path );
}

}
