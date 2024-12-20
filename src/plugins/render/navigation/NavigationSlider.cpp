// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "NavigationSlider.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPixmapCache>

namespace Marble
{

namespace
{
const int handleImageHeight = 32;
}

NavigationSlider::NavigationSlider(QWidget *parent)
    : QAbstractSlider(parent)
    , m_handleImagePath(QStringLiteral("marble/navigation/navigational_slider_handle"))
{
    setMouseTracking(true);
}

NavigationSlider::~NavigationSlider()
{
    QPixmapCache::remove(QStringLiteral("marble/navigation/navigational_slider_groove"));
    QPixmapCache::remove(QStringLiteral("marble/navigation/navigational_slider_handle"));
    QPixmapCache::remove(QStringLiteral("marble/navigation/navigational_slider_handle_hover"));
    QPixmapCache::remove(QStringLiteral("marble/navigation/navigational_slider_handle_press"));
}

QPixmap NavigationSlider::pixmap(const QString &id)
{
    QPixmap result;
    if (!QPixmapCache::find(id, &result)) {
        result = QPixmap(QLatin1StringView(":/") + id + QLatin1StringView(".png"));
        QPixmapCache::insert(id, result);
    }
    return result;
}

void NavigationSlider::enterEvent(QEnterEvent *)
{
    setSliderDown(false);
    if (m_handleImagePath != QLatin1StringView("marble/navigation/navigational_slider_handle_hover")) {
        m_handleImagePath = QStringLiteral("marble/navigation/navigational_slider_handle_hover");
        repaint();
    }
}

void NavigationSlider::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if (!isSliderDown() && mouseEvent->buttons() & Qt::LeftButton) {
        setSliderDown(true);
    }
    if (isSliderDown()) {
        qreal const fraction = (mouseEvent->pos().y() - handleImageHeight / 2) / qreal(height() - handleImageHeight);
        int v = (int)minimum() + ((maximum() - minimum())) * (1 - fraction);
        setValue(v);
        repaint();
    }
}

void NavigationSlider::mousePressEvent(QMouseEvent *)
{
    setSliderDown(true);
    if (m_handleImagePath != QLatin1StringView("marble/navigation/navigational_slider_handle_press")) {
        m_handleImagePath = QStringLiteral("marble/navigation/navigational_slider_handle_press");
        repaint();
    }
}

void NavigationSlider::mouseReleaseEvent(QMouseEvent *)
{
    setSliderDown(false);
    if (m_handleImagePath != QLatin1StringView("marble/navigation/navigational_slider_handle_hover")) {
        m_handleImagePath = QStringLiteral("marble/navigation/navigational_slider_handle_hover");
        repaint();
    }
}

void NavigationSlider::leaveEvent(QEvent *)
{
    setSliderDown(false);
    if (m_handleImagePath != QLatin1StringView("marble/navigation/navigational_slider_handle")) {
        m_handleImagePath = QStringLiteral("marble/navigation/navigational_slider_handle");
        repaint();
    }
}

void NavigationSlider::repaint()
{
    Q_EMIT repaintNeeded();
}

void NavigationSlider::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    for (int y = 0; y <= 160; y += 10) {
        painter.drawPixmap(0, y, pixmap(QStringLiteral("marble/navigation/navigational_slider_groove")));
    }
    qreal const fraction = (value() - minimum()) / qreal(maximum() - minimum());
    int const y = (height() - handleImageHeight) * (1 - fraction);
    painter.drawPixmap(0, y, pixmap(m_handleImagePath));
    painter.end();
}

}

#include "moc_NavigationSlider.cpp"
