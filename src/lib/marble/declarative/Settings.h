// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_SETTINGS_H
#define MARBLE_DECLARATIVE_SETTINGS_H

#include <QObject>
#include <QVariant>
#include <qqmlregistration.h>

class Settings : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString organizationName READ organizationName WRITE setOrganizationName)
    Q_PROPERTY(QString applicationName READ applicationName WRITE setApplicationName)
    Q_PROPERTY(bool debugOutputEnabled READ debugOutputEnabled WRITE setDebugOutputEnabled NOTIFY debugOutputEnabledChanged)

public:
    Settings();

    QString organizationName() const;

    void setOrganizationName(const QString &organization);

    QString applicationName() const;

    void setApplicationName(const QString &application);

    bool debugOutputEnabled() const;

    Q_INVOKABLE QVariant value(const QString &group, const QString &key, const QVariant &value = QVariant()) const;

    Q_INVOKABLE void setValue(const QString &group, const QString &key, const QVariant &value);

    Q_INVOKABLE void remove(const QString &group, const QString &value);

    Q_INVOKABLE void setDebugOutputEnabled(bool debugOutputEnabled);

Q_SIGNALS:
    void debugOutputEnabledChanged(bool debugOutputEnabled);

private:
    QString m_organizationName;
    QString m_applicationName;
};

#endif // MARBLE_DECLARATIVE_SETTINGS_H
