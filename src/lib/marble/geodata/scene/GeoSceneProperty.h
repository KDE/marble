/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEPROPERTY_H
#define MARBLE_GEOSCENEPROPERTY_H

#include <QObject>
#include <QString>

#include <geodata_export.h>

#include "GeoDocument.h"

namespace Marble
{

/**
 * @short Settings property within a GeoScene document.
 */

class GEODATA_EXPORT GeoSceneProperty : public QObject, public GeoNode
{
    Q_OBJECT

public:
    explicit GeoSceneProperty(const QString &name);
    const char *nodeType() const override;

    QString name() const;
    bool available() const;
    void setAvailable(bool available);
    bool defaultValue() const;
    void setDefaultValue(bool defaultValue);
    bool value() const;
    void setValue(bool value);

Q_SIGNALS:
    void valueChanged(const QString &, bool);

private:
    // FIXME: d-pointerfy

    QString m_name;
    bool m_available;
    bool m_defaultValue;
    bool m_value;
};

}

#endif
