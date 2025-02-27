// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Utku Aydın <utkuaydin34@gmail.com>
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "OpenDesktopItem.h"
#include "layers/PopupLayer.h"
#include <QPainter>

#include <QAction>

using namespace Marble;

OpenDesktopItem::OpenDesktopItem(QObject *parent)
    : AbstractDataPluginItem(parent)
    , m_action(new QAction(this))
{
    connect(m_action, &QAction::triggered, this, &OpenDesktopItem::openBrowser);
    setCacheMode(ItemCoordinateCache);
}

OpenDesktopItem::~OpenDesktopItem()
{
    delete m_action;
}

bool OpenDesktopItem::initialized() const
{
    return size() != QSizeF(-1, -1);
}

bool OpenDesktopItem::operator<(const AbstractDataPluginItem *other) const
{
    // Custom avatars will have more priority than default ones
    QString noAvatar = QStringLiteral("http://opendesktop.org/usermanager/nopic.png");
    const auto item = dynamic_cast<const OpenDesktopItem *>(other);

    if (item) {
        if (this->avatarUrl().toString() == noAvatar && item->avatarUrl().toString() != noAvatar)
            return false;

        else if (this->avatarUrl().toString() != noAvatar && item->avatarUrl().toString() == noAvatar)
            return true;
    }

    return this > other;
}

void OpenDesktopItem::addDownloadedFile(const QString &url, const QString &type)
{
    if (type == QLatin1StringView("avatar")) {
        m_pixmap.load(url);
        setSize(m_pixmap.size());
        Q_EMIT updated();
    }
}

void OpenDesktopItem::paint(QPainter *painter)
{
    painter->drawPixmap(0, 0, m_pixmap);
}

void OpenDesktopItem::updateToolTip()
{
    const QString toolTip = QLatin1StringView(
        "<html><head><meta name='qrichtext' content='1' />"
        "<style type='text/css'>"
        "body { font-family:'Sans Serif'; font-size:9pt; font-weight:400; }"
        "np, li { white-space: pre-wrap; }"
        "p { margin: 0; -qt-block-indent:0; text-indent:0px }"
        "</style></head><body><table>"
        "<tr><td align='right'>Fullname:</td><td>%1</td></tr>"
        "<tr><td align='right'>Location:</td><td>%2</td></tr>"
        "<tr><td align='right'>Role:</td><td>%3</td></tr>"
        "</table></body></html>");
    setToolTip(toolTip.arg(fullName()).arg(location()).arg(role()));
}

QAction *OpenDesktopItem::action()
{
    m_action->setText(id());
    return m_action;
}

void OpenDesktopItem::openBrowser()
{
    PopupLayer *popup = m_marbleWidget->popupLayer();
    popup->setCoordinates(coordinate(), Qt::AlignRight | Qt::AlignVCenter);
    popup->setUrl(profileUrl());
    popup->setSize(QSizeF(900, 600));
    popup->popup();
}

QUrl OpenDesktopItem::profileUrl() const
{
    return QUrl(QStringLiteral("http://opendesktop.org/usermanager/search.php?username=%1").arg(id()));
}

QUrl OpenDesktopItem::avatarUrl() const
{
    return m_avatarUrl;
}

void OpenDesktopItem::setAvatarUrl(const QUrl &url)
{
    m_avatarUrl = url;
}

QString OpenDesktopItem::fullName() const
{
    return m_fullName;
}

void OpenDesktopItem::setFullName(const QString &fullName)
{
    m_fullName = fullName;
    updateToolTip();
}

QString OpenDesktopItem::location() const
{
    return m_location;
}

void OpenDesktopItem::setLocation(const QString &location)
{
    m_location = location;
    updateToolTip();
}

QString OpenDesktopItem::role() const
{
    return m_role;
}

void OpenDesktopItem::setRole(const QString &role)
{
    m_role = role;
    updateToolTip();
}

void OpenDesktopItem::setMarbleWidget(MarbleWidget *widget)
{
    m_marbleWidget = widget;
}

// This is needed for all QObjects (see MOC)
#include "moc_OpenDesktopItem.cpp"
