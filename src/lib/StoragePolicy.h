//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig <tokoe@kde.org>"
//

#ifndef STORAGEPOLICY_H
#define STORAGEPOLICY_H


#include <QtCore/QString>


class QByteArray;


class StoragePolicy
{
    public:
        virtual ~StoragePolicy() {}

        virtual bool fileExists( const QString &fileName ) const = 0;

        virtual bool updateFile( const QString &fileName, const QByteArray &data ) = 0;

	virtual void clearCache() = 0;

        virtual QString lastErrorMessage() const = 0;
};

#endif
