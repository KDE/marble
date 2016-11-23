//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_SETTINGS_H
#define MARBLE_DECLARATIVE_SETTINGS_H

#include <QObject>
#include <QVariant>

class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString organizationName READ organizationName WRITE setOrganizationName )
    Q_PROPERTY( QString applicationName READ applicationName WRITE setApplicationName )
    Q_PROPERTY( bool debugOutputEnabled READ debugOutputEnabled WRITE setDebugOutputEnabled NOTIFY debugOutputEnabledChanged)

public:
    Settings();

    QString organizationName() const;

    void setOrganizationName( const QString &organization );

    QString applicationName() const;

    void setApplicationName( const QString &application );

    bool debugOutputEnabled() const;

public Q_SLOTS:
    QVariant value( const QString &group, const QString &key, const QVariant &value = QVariant() ) const;

    void setValue( const QString &group, const QString &key, const QVariant &value );

    void remove(const QString &group, const QString &value);

    void setDebugOutputEnabled(bool debugOutputEnabled);

Q_SIGNALS:
    void debugOutputEnabledChanged(bool debugOutputEnabled);

private:
    QString m_organizationName;
    QString m_applicationName;
};

#endif // MARBLE_DECLARATIVE_SETTINGS_H
