// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DBUSINTERFACE_H
#define MARBLE_DBUSINTERFACE_H

#include "marble_export.h"

namespace Marble
{
class MarbleWidget;
}

#include <QDBusAbstractAdaptor>
#include <QPointF>
#include <QScopedPointer>
#include <QStringList>

namespace Marble
{

class MARBLE_EXPORT MarbleDBusInterface : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.marble")

    Q_PROPERTY(QString mapTheme READ mapTheme WRITE setMapTheme NOTIFY mapThemeChanged)
    Q_PROPERTY(QStringList mapThemes READ mapThemes)
    Q_PROPERTY(int tileLevel READ tileLevel NOTIFY tileLevelChanged)
    Q_PROPERTY(int zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(QStringList properties READ properties)
    Q_PROPERTY(QPointF center READ center WRITE setCenter NOTIFY centerChanged)

public:
    explicit MarbleDBusInterface(MarbleWidget *widget);
    ~MarbleDBusInterface() override;

    QString mapTheme() const;
    QStringList mapThemes() const;
    int tileLevel() const;
    int zoom() const;
    QPointF center() const;

public Q_SLOTS:
    void setMapTheme(const QString &mapTheme);
    void setZoom(int zoom);
    QStringList properties() const;
    void setCenter(const QPointF &center) const;

public Q_SLOTS:
    Q_INVOKABLE void setPropertyEnabled(const QString &key, bool enabled);
    Q_INVOKABLE bool isPropertyEnabled(const QString &key) const;

Q_SIGNALS:
    void mapThemeChanged(const QString &mapTheme);
    void tileLevelChanged(int tileLevel);
    void zoomChanged(int zoom);
    void centerChanged(const QPointF &center);

private Q_SLOTS:
    void handleVisibleLatLonAltBoxChange();

private:
    Q_DISABLE_COPY(MarbleDBusInterface)
    class Private;
    friend class Private;
    QScopedPointer<Private> const d;
};

}

#endif
