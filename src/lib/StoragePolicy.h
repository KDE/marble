//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig <tokoe@kde.org>"
// Copyright 2009      Bastian Holst <bastianholst@gmx.de
//

#ifndef STORAGEPOLICY_H
#define STORAGEPOLICY_H


#include <QtCore/QObject>
#include <QtCore/QString>


class QByteArray;

namespace Marble
{

class StoragePolicy : public QObject
{
    Q_OBJECT
    
    public:
	StoragePolicy( QObject *parent = 0 );
	
        virtual ~StoragePolicy() {}

        virtual bool fileExists( const QString &fileName ) const = 0;

        virtual bool updateFile( const QString &fileName, const QByteArray &data ) = 0;

	virtual void clearCache() = 0;

        virtual QString lastErrorMessage() const = 0;
	
    Q_SIGNALS:
	void cleared();
	void sizeChanged( qint64 );
	
    private:
	Q_DISABLE_COPY( StoragePolicy )
};

}

#endif
