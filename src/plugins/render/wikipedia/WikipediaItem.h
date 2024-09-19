// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef WIKIPEDIAITEM_H
#define WIKIPEDIAITEM_H

#include "AbstractDataPluginItem.h"

#include <QHash>
#include <QIcon>
#include <QPixmap>
#include <QUrl>

class QAction;

namespace Marble
{

class MarbleWidget;
class TinyWebBrowser;

class WikipediaItem : public AbstractDataPluginItem
{
    Q_OBJECT

public:
    WikipediaItem(MarbleWidget *widget, QObject *parent);

    ~WikipediaItem() override;

    QString name() const;

    void setName(const QString &name);

    bool initialized() const override;

    void addDownloadedFile(const QString &url, const QString &type) override;

    void paint(QPainter *painter) override;

    bool operator<(const AbstractDataPluginItem *other) const override;

    qreal longitude() const;

    void setLongitude(qreal longitude);

    qreal latitude() const;

    void setLatitude(qreal latitude);

    QUrl url() const;

    void setUrl(const QUrl &url);

    QUrl thumbnailImageUrl() const;

    void setThumbnailImageUrl(const QUrl &thumbnailImageUrl);

    QString summary() const;

    void setSummary(const QString &summary);

    QAction *action() override;

    void setIcon(const QIcon &icon);

    void setSettings(const QHash<QString, QVariant> &settings) override;

    /** Set a popularity rank. Larger means more popular. Default rank is 0 */
    void setRank(double rank);

    double rank() const;

public Q_SLOTS:
    void openBrowser();

private:
    void updateSize();
    void updateToolTip();
    bool showThumbnail() const;

    MarbleWidget *m_marbleWidget;
    QUrl m_url;
    QUrl m_thumbnailImageUrl;
    QString m_summary;
    double m_rank;
    TinyWebBrowser *m_browser;
    QAction *m_action;

    QPixmap m_thumbnail;
    QIcon m_wikiIcon;
    bool m_showThumbnail;
};

}

#endif // WIKIPEDIAITEM_H
