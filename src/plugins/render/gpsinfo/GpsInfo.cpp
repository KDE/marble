// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>
//

#include "GpsInfo.h"

#include <QIcon>

#include "GeoDataAccuracy.h"
#include "MarbleDebug.h"
#include "MarbleGraphicsGridLayout.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "PositionTracking.h"
#include "ViewportParams.h"
#include "WidgetGraphicsItem.h"

namespace Marble
{

GpsInfo::GpsInfo()
    : AbstractFloatItem(nullptr)
    , m_locale(nullptr)
    , m_widgetItem(nullptr)
{
}

GpsInfo::GpsInfo(const MarbleModel *marbleModel)
    : AbstractFloatItem(marbleModel, QPointF(10.5, 110), QSizeF(135.0, 80.0))
    , m_locale(nullptr)
    , m_widgetItem(nullptr)
{
    setVisible(false);
}

GpsInfo::~GpsInfo()
{
}

QStringList GpsInfo::backendTypes() const
{
    return QStringList(QStringLiteral("GpsInfo"));
}

QString GpsInfo::name() const
{
    return tr("GpsInfo");
}

QString GpsInfo::guiString() const
{
    return tr("&GpsInfo");
}

QString GpsInfo::nameId() const
{
    return QStringLiteral("GpsInfo");
}

QString GpsInfo::version() const
{
    return QStringLiteral("1.0");
}

QString GpsInfo::description() const
{
    return tr("This is a float item that provides Gps Information.");
}

QString GpsInfo::copyrightYears() const
{
    return QStringLiteral("2011");
}

QVector<PluginAuthor> GpsInfo::pluginAuthors() const
{
    return QVector<PluginAuthor>() << PluginAuthor(QStringLiteral("Thibaut Gridel"), QStringLiteral("tgridel@free.fr"));
}

QIcon GpsInfo::icon() const
{
    return QIcon(QStringLiteral(":/icons/gps.png"));
}

void GpsInfo::initialize()
{
    if (!m_widgetItem) {
        QWidget *widget = new QWidget;
        m_widget.setupUi(widget);
        m_widgetItem = new WidgetGraphicsItem(this);
        m_widgetItem->setWidget(widget);

        MarbleGraphicsGridLayout *layout = new MarbleGraphicsGridLayout(1, 1);
        layout->addItem(m_widgetItem, 0, 0);
        setLayout(layout);
        setPadding(0);

        m_locale = MarbleGlobal::getInstance()->locale();
        connect(marbleModel()->positionTracking(), SIGNAL(gpsLocation(GeoDataCoordinates, qreal)), this, SLOT(updateLocation(GeoDataCoordinates, qreal)));
    }
}

bool GpsInfo::isInitialized() const
{
    return m_widgetItem;
}

void GpsInfo::updateLocation(const GeoDataCoordinates &coordinates, qreal)
{
    PositionTracking *tracking = marbleModel()->positionTracking();
    qreal speed = tracking->speed();
    qreal direction = tracking->direction();
    qreal altitude = coordinates.altitude();
    qreal precision = tracking->accuracy().horizontal;
    QString speedString;
    QString distanceString;

    switch (m_locale->measurementSystem()) {
    case MarbleLocale::ImperialSystem:
        // miles per hour
        speedString = tr("mph");
        speed *= HOUR2SEC * METER2KM * KM2MI;
        distanceString = tr("ft");
        altitude *= M2FT;
        precision *= M2FT;
        break;

    case MarbleLocale::MetricSystem:
        // kilometers per hour
        speedString = tr("km/h");
        speed *= HOUR2SEC * METER2KM;
        distanceString = tr("m");
        break;

    case MarbleLocale::NauticalSystem:
        // nm per hour (knots)
        speedString = tr("kt");
        speed *= HOUR2SEC * METER2KM * KM2NM;
        distanceString = tr("nm");
        break;
    }

    m_widget.SpeedValue->setText(QStringLiteral(" %1 %2").arg(QLocale().toString(speed, 'f', 1)).arg(speedString));
    m_widget.AltitudeValue->setText(QStringLiteral(" %1 %2").arg(QLocale().toString(altitude, 'f', 1)).arg(distanceString));
    m_widget.DirectionValue->setText(QStringLiteral(" %1 %2").arg(QLocale().toString(direction, 'f', 1)).arg("d"));
    m_widget.PrecisionValue->setText(QStringLiteral(" %1 %2").arg(QLocale().toString(precision, 'f', 1)).arg(distanceString));

    int const minimumWidth = m_widgetItem->widget()->sizeHint().width();
    if (size().width() < minimumWidth) {
        m_widgetItem->setSize(QSizeF(minimumWidth, size().height()));
    }

    update();
    emit repaintNeeded();
}

}

#include "moc_GpsInfo.cpp"
