// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Utku Aydin <utkuaydin34@gmail.com>
//

#ifndef EARTHQUAKEITEM_H
#define EARTHQUAKEITEM_H

#include "AbstractDataPluginItem.h"

#include <QDateTime>
#include <QFont>

namespace Marble
{

class EarthquakeItem : public AbstractDataPluginItem
{
    Q_OBJECT

public:
    explicit EarthquakeItem(QObject *parent);

    ~EarthquakeItem() override;

    // Returns true if the item is paintable
    bool initialized() const override;

    // Here the item gets painted
    void paint(QPainter *painter) override;

    bool operator<(const AbstractDataPluginItem *other) const override;

    // Magnitude whose value we use to determine circle's diameter
    double magnitude() const;

    void setMagnitude(double magnitude);

    void setDateTime(const QDateTime &dateTime);

    QDateTime dateTime() const;

    /** Earthquake's depth in km */
    double depth() const;

    void setDepth(double depth);

private:
    void updateTooltip();

    double m_magnitude;

    double m_depth;

    QDateTime m_dateTime;

    static const QFont s_font;
};

}
#endif // EARTHQUAKEITEM_H
