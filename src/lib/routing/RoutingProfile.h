//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ROUTINGPROFILE_H
#define MARBLE_ROUTINGPROFILE_H

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QVariant>

namespace Marble
{

class RoutingProfile {
public:
    explicit RoutingProfile( const QString &name = QString() );

    QString name() const;

    void setName( const QString &name );

    const QHash<QString, QHash<QString, QVariant> >& pluginSettings() const;

    QHash<QString, QHash<QString, QVariant> >& pluginSettings();

private:
    QString m_name;
    //icon
    QHash<QString, QHash<QString, QVariant> > m_pluginSettings;
};

}

#endif
