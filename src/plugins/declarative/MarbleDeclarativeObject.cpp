//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MarbleDeclarativeObject.h"

#include "MarbleDirs.h"
#include "MarbleGlobal.h"

#include <QtCore/QProcessEnvironment>

class MarbleDeclarativeObjectPrivate
{
public:
    // Yet empty
};

MarbleDeclarativeObject::MarbleDeclarativeObject( QObject *parent ) :
    QObject( parent ),
    d( new MarbleDeclarativeObjectPrivate )
{
    // nothing to do
}

MarbleDeclarativeObject::~MarbleDeclarativeObject()
{
    delete d;
}

QString MarbleDeclarativeObject::version() const
{
    return Marble::MARBLE_VERSION_STRING;
}

QString MarbleDeclarativeObject::resolvePath(const QString &path) const
{
    return Marble::MarbleDirs::path( path );
}

bool MarbleDeclarativeObject::canExecute(const QString &program) const
{
    QString path = QProcessEnvironment::systemEnvironment().value( "PATH", "/usr/local/bin:/usr/bin:/bin" );
    foreach( const QString &dir, path.split( ":" ) ) {
        QFileInfo const executable( QDir( dir ), program );
        if ( executable.exists() && executable.isExecutable() ) {
            return true;
        }
    }

    return false;
}

#include "MarbleDeclarativeObject.moc"
