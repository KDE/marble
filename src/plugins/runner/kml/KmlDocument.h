//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Dennis Nienhüser <nienhueser@kde.org>

#ifndef MARBLE_KMLDOCUMENT_H
#define MARBLE_KMLDOCUMENT_H

#include "GeoDataDocument.h"

#include <QStringList>

namespace Marble {

class KmlDocument : public GeoDataDocument
{
public:
    ~KmlDocument();

    /**
     * @brief Temporary files to delete at destruction
     * @param path Empty subdirectories below this directory will be deleted at destruction
     * @param files List of files to delete at destruction
     */
    void setFiles( const QString &path, const QStringList &files );

private:
    void cleanupTemporaryFiles( const QString &path );
    void removeDirectoryRecursively( const QString &path );

    QString m_path;
    QStringList m_files;
};

}

#endif
