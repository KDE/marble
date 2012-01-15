//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_SETTINGS_H
#define MARBLE_DECLARATIVE_SETTINGS_H

#include <QtCore/QObject>
#include <QtCore/QSettings>

class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString organizationName READ organizationName WRITE setOrganizationName )
    Q_PROPERTY( QString applicationName READ applicationName WRITE setApplicationName )

public:
    Settings();

    QString organizationName() const;

    void setOrganizationName( const QString &organization );

    QString applicationName() const;

    void setApplicationName( const QString &application );

public Q_SLOTS:
    QVariant value( const QString &group, const QString &key, const QVariant &value = QVariant() ) const;

    void setValue( const QString &group, const QString &key, const QVariant &value );

private:
    QString m_organizationName;

    QString m_applicationName;
};

#endif // MARBLE_DECLARATIVE_SETTINGS_H
