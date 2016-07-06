//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <nienhueser@kde.org>

#ifndef MARBLE_KMZHANDLER_H
#define MARBLE_KMZHANDLER_H

#include <QStringList>

namespace Marble {

class KmzHandler
{
public:
    bool open(const QString &file, QString &error);

    QString kmlFile() const;

    QString kmzPath() const;

    QStringList kmzFiles() const;

private:
    QString m_kmlFile;
    QString m_kmzPath;
    QStringList m_kmzFiles;
};

}

#endif
