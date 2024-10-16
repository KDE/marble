// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef OPENDESKTOPITEM_H
#define OPENDESKTOPITEM_H

#include "AbstractDataPluginItem.h"
#include "MarbleWidget.h"

#include <QUrl>

class QAction;

namespace Marble
{

class OpenDesktopItem : public AbstractDataPluginItem
{
    Q_OBJECT

public:
    explicit OpenDesktopItem(QObject *parent);

    ~OpenDesktopItem() override;

    bool initialized() const override;

    void addDownloadedFile(const QString &url, const QString &type) override;

    void paint(QPainter *painter) override;

    bool operator<(const AbstractDataPluginItem *other) const override;

    void updateToolTip();

    QAction *action() override;

    QUrl profileUrl() const;

    QUrl avatarUrl() const;

    void setAvatarUrl(const QUrl &url);

    QString fullName() const;

    void setFullName(const QString &fullName);

    QString location() const;

    void setLocation(const QString &location);

    QString role() const;

    void setRole(const QString &role);

    void setMarbleWidget(MarbleWidget *widget);

public Q_SLOTS:
    void openBrowser();

private:
    MarbleWidget *m_marbleWidget = nullptr;
    QString m_fullName;
    QString m_location;
    QString m_role;
    QPixmap m_pixmap;
    QUrl m_avatarUrl;
    QAction *const m_action;
};

}
#endif // OPENDESKTOPITEM_H
