// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2013 Yazeed Zoabi <yazeedz.zoabi@gmail.com>
//

//
// The Legend Browser displays the legend
//

#ifndef MARBLE_MARBLELEGENDBROWSER_H
#define MARBLE_MARBLELEGENDBROWSER_H

#ifdef MARBLE_NO_WEBKITWIDGETS
#include "NullMarbleWebView.h"
#else
#include "MarbleWebView.h"
#endif

#include "marble_export.h"

class QEvent;
class QUrl;
class QString;

namespace Marble
{

class MarbleModel;
class MarbleLegendBrowserPrivate;

class MARBLE_EXPORT MarbleLegendBrowser : public MarbleWebView
{
    Q_OBJECT

public:
    explicit MarbleLegendBrowser(QWidget *parent);
    ~MarbleLegendBrowser() override;

    void setMarbleModel(MarbleModel *marbleModel);
    QSize sizeHint() const override;

Q_SIGNALS:
    void toggledShowProperty(const QString &, bool);
    void tourLinkClicked(const QString &url);

public Q_SLOTS:
    void setCheckedProperty(const QString &name, bool checked);
    void setRadioCheckedProperty(const QString &value, const QString &name, bool checked);

private Q_SLOTS:
    void initTheme();
    void loadLegend();
    void openLinkExternally(const QUrl &url);

protected:
    bool event(QEvent *event) override;
    static QString readHtml(const QUrl &name);
    QString generateSectionsHtml();
    static void translateHtml(QString &html);

private:
    void injectWebChannel(QString &html);
    void reverseSupportCheckboxes(QString &html);

private:
    Q_DISABLE_COPY(MarbleLegendBrowser)
    MarbleLegendBrowserPrivate *const d;
};

class MarbleJsWrapper : public QObject
{
    Q_OBJECT
public:
    explicit MarbleJsWrapper(MarbleLegendBrowser *parent)
        : m_parent(parent)
    {
    }

public Q_SLOTS:
    void setCheckedProperty(const QString &name, bool checked)
    {
        m_parent->setCheckedProperty(name, checked);
    }
    void setRadioCheckedProperty(const QString &value, const QString &name, bool checked)
    {
        m_parent->setRadioCheckedProperty(value, name, checked);
    }

private:
    MarbleLegendBrowser *m_parent;
};

}

#endif
