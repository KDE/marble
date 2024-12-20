// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Utku Aydın <utkuaydin34@gmail.com>
//

#include "FoursquareItem.h"
#include "GeoPainter.h"
#include "ViewportParams.h"

#include <QFontMetrics>
#include <QPainterPath>

namespace Marble
{

QFont FoursquareItem::s_font = QFont(QStringLiteral("Sans Serif"), 8);

FoursquareItem::FoursquareItem(QObject *parent)
    : AbstractDataPluginItem(parent)
    , m_usersCount(0)
{
    setSize(QSize(0, 0));
}

FoursquareItem::~FoursquareItem() = default;

bool FoursquareItem::initialized() const
{
    // Find something logical for this
    return true;
}

bool FoursquareItem::operator<(const AbstractDataPluginItem *other) const
{
    const auto item = dynamic_cast<const FoursquareItem *>(other);
    return item && this->usersCount() > item->usersCount();
}

QString FoursquareItem::name() const
{
    return m_name;
}

void FoursquareItem::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        QFontMetrics const fontMetrics(s_font);
        setSize(QSizeF(fontMetrics.horizontalAdvance(m_name) + 10, fontMetrics.height() + 10));
        Q_EMIT nameChanged();
    }
}

QString FoursquareItem::category() const
{
    return m_category;
}

void FoursquareItem::setCategory(const QString &category)
{
    if (category != m_category) {
        m_category = category;
        Q_EMIT categoryChanged();
    }
}

QString FoursquareItem::address() const
{
    return m_address;
}

void FoursquareItem::setAddress(const QString &address)
{
    if (address != m_address) {
        m_address = address;
        Q_EMIT addressChanged();
    }
}

QString FoursquareItem::city() const
{
    return m_city;
}
void FoursquareItem::setCity(const QString &city)
{
    if (city != m_city) {
        m_city = city;
        Q_EMIT cityChanged();
    }
}

QString FoursquareItem::country() const
{
    return m_country;
}

void FoursquareItem::setCountry(const QString &country)
{
    if (country != m_country) {
        m_country = country;
        Q_EMIT countryChanged();
    }
}

int FoursquareItem::usersCount() const
{
    return m_usersCount;
}

void FoursquareItem::setUsersCount(const int count)
{
    if (count != m_usersCount) {
        m_usersCount = count;
        Q_EMIT usersCountChanged();
    }
}

QString FoursquareItem::categoryIconUrl() const
{
    return m_categoryIconUrl;
}

void FoursquareItem::setCategoryIconUrl(const QString &url)
{
    if (url != m_categoryIconUrl) {
        m_categoryIconUrl = url;
        Q_EMIT categoryIconUrlChanged();
    }
}

QString FoursquareItem::categoryLargeIconUrl() const
{
    return m_categoryLargeIconUrl;
}

void FoursquareItem::setCategoryLargeIconUrl(const QString &url)
{
    if (url != m_categoryLargeIconUrl) {
        m_categoryLargeIconUrl = url;
        Q_EMIT categoryLargeIconUrlChanged();
    }
}

void FoursquareItem::paint(QPainter *painter)
{
    // Save the old painter state.
    painter->save();
    painter->setPen(QPen(QColor(Qt::white)));
    painter->setFont(s_font);

    // Draw the text into the given rect.
    QRect rect = QRect(QPoint(0, 0), size().toSize());
    QRect boundingRect = QRect(QPoint(rect.top(), rect.left()), QSize(rect.width(), rect.height()));
    QPainterPath painterPath;
    painterPath.addRoundedRect(boundingRect, 5, 5);
    painter->setClipPath(painterPath);
    painter->drawPath(painterPath);
    painter->fillPath(painterPath, QBrush(QColor(0x39AC39)));
    painter->drawText(rect.adjusted(5, 5, -5, -5), 0, m_name);

    painter->restore();
}

}

#include "moc_FoursquareItem.cpp"
