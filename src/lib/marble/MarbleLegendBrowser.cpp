// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
// SPDX-FileCopyrightText: 2013 Yazeed Zoabi <yazeedz.zoabi@gmail.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "MarbleLegendBrowser.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QEvent>
#include <QFile>
#include <QMouseEvent>
#include <QPainter>
#include <QRegularExpression>
#include <QUrl>

#ifndef MARBLE_NO_WEBKITWIDGETS
#include <QWebChannel>
#include <QWebEnginePage>
#endif

#include <QTextDocument>

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneIcon.h"
#include "GeoSceneItem.h"
#include "GeoSceneLegend.h"
#include "GeoSceneProperty.h"
#include "GeoSceneSection.h"
#include "GeoSceneSettings.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "TemplateDocument.h"

namespace Marble
{

class MarbleLegendBrowserPrivate
{
public:
    MarbleModel *m_marbleModel;
    QMap<QString, bool> m_checkBoxMap;
    QMap<QString, QPixmap> m_symbolMap;
    QString m_currentThemeId;
    MarbleJsWrapper *m_jsWrapper;
};

// ================================================================

MarbleLegendBrowser::MarbleLegendBrowser(QWidget *parent)
    : MarbleWebView(parent)
    , d(new MarbleLegendBrowserPrivate)
{
    d->m_marbleModel = nullptr;
    d->m_jsWrapper = new MarbleJsWrapper(this);
}

MarbleLegendBrowser::~MarbleLegendBrowser()
{
    delete d;
}

void MarbleLegendBrowser::setMarbleModel(MarbleModel *marbleModel)
{
    // We need this to be able to get to the MapTheme.
    d->m_marbleModel = marbleModel;

    if (d->m_marbleModel) {
        connect(d->m_marbleModel, &MarbleModel::themeChanged, this, &MarbleLegendBrowser::initTheme);
    }
}

QSize MarbleLegendBrowser::sizeHint() const
{
    return {320, 320};
}

void MarbleLegendBrowser::initTheme()
{
    // Check for a theme specific legend.html first
    if (d->m_marbleModel != nullptr && d->m_marbleModel->mapTheme() != nullptr) {
        const GeoSceneDocument *currentMapTheme = d->m_marbleModel->mapTheme();

        d->m_checkBoxMap.clear();

        for (const GeoSceneProperty *property : currentMapTheme->settings()->allProperties()) {
            if (property->available()) {
                d->m_checkBoxMap[property->name()] = property->value();
            }
        }

        disconnect(currentMapTheme, SIGNAL(valueChanged(QString, bool)), nullptr, nullptr);
        connect(currentMapTheme, SIGNAL(valueChanged(QString, bool)), this, SLOT(setCheckedProperty(QString, bool)));
    }

    if (isVisible()) {
        loadLegend();
    }
}

void MarbleLegendBrowser::loadLegend()
{
    if (!d->m_marbleModel) {
        return;
    }

#ifndef MARBLE_NO_WEBKITWIDGETS
    if (d->m_currentThemeId != d->m_marbleModel->mapThemeId()) {
        d->m_currentThemeId = d->m_marbleModel->mapThemeId();
    } else {
        return;
    }

    // Read the html string.
    QString legendPath;

    // Check for a theme specific legend.html first
    if (d->m_marbleModel->mapTheme() != nullptr) {
        const GeoSceneDocument *currentMapTheme = d->m_marbleModel->mapTheme();

        legendPath = MarbleDirs::path(QLatin1StringView("maps/") + currentMapTheme->head()->target() + QLatin1Char('/') + currentMapTheme->head()->theme()
                                      + QLatin1StringView("/legend.html"));
    }
    if (legendPath.isEmpty()) {
        legendPath = MarbleDirs::path(QStringLiteral("legend.html"));
    }

    QString finalHtml = readHtml(QUrl::fromLocalFile(legendPath));

    TemplateDocument doc(finalHtml);
    finalHtml = doc.finalText();

    injectWebChannel(finalHtml);
    reverseSupportCheckboxes(finalHtml);

    // Generate some parts of the html from the MapTheme <Legend> tag.
    const QString sectionsHtml = generateSectionsHtml();

    // And then create the final html from these two parts.
    finalHtml.replace(QStringLiteral("<!-- ##customLegendEntries:all## -->"), sectionsHtml);

    translateHtml(finalHtml);

    QUrl baseUrl = QUrl::fromLocalFile(legendPath);

    // Set the html string in the QTextBrowser.
    auto page = new MarbleWebPage(this);
    connect(page, SIGNAL(linkClicked(QUrl)), this, SLOT(openLinkExternally(QUrl)));
    page->setHtml(finalHtml, baseUrl);
    setPage(page);

    auto channel = new QWebChannel(page);
    channel->registerObject(QStringLiteral("Marble"), d->m_jsWrapper);
    page->setWebChannel(channel);

    if (d->m_marbleModel) {
        page->toHtml([=](QString document) {
            d->m_marbleModel->setLegend(new QTextDocument(document));
        });
    }
#endif
}

void MarbleLegendBrowser::openLinkExternally(const QUrl &url)
{
    if (url.scheme() == QLatin1StringView("tour")) {
        Q_EMIT tourLinkClicked(QLatin1StringView("maps/") + url.host() + url.path());
    } else {
        QDesktopServices::openUrl(url);
    }
}

bool MarbleLegendBrowser::event(QEvent *event)
{
    // "Delayed initialization": legend gets created only
    if (event->type() == QEvent::Show) {
        loadLegend();
    }

    return MarbleWebView::event(event);
}

QString MarbleLegendBrowser::readHtml(const QUrl &name)
{
    QString html;

    QFile data(name.toLocalFile());
    if (data.open(QFile::ReadOnly)) {
        QTextStream in(&data);
        html = in.readAll();
        data.close();
    }

    return html;
}

void MarbleLegendBrowser::translateHtml(QString &html)
{
    // must match string extraction in Messages.sh
    QString s = html;
    QRegularExpression rx(QStringLiteral(R"(</?\w+((\s+\w+(\s*=\s*(?:".*"|'.*'|[^'">\s]+))?)+\s*|\s*)/?>)"),
                          QRegularExpression::InvertedGreedinessOption); // PORT_QT6: double check
    /*
        QRegExp rx( "</?\\w+((\\s+\\w+(\\s*=\\s*(?:\".*\"|'.*'|[^'\">\\s]+))?)+\\s*|\\s*)/?>");
        rx.setMinimal( true );
    */
    s.replace(rx, QLatin1StringView("\n"));
    s.replace(QRegularExpression(QStringLiteral("\\s*\n\\s*")), QStringLiteral("\n"));
    const QStringList words = s.split(QLatin1Char('\n'), Qt::SkipEmptyParts);

    QStringList::const_iterator i = words.constBegin();
    QStringList::const_iterator const end = words.constEnd();
    for (; i != end; ++i)
        html.replace(*i, QCoreApplication::translate("Legends", (*i).toUtf8().constData()));
}

void MarbleLegendBrowser::injectWebChannel(QString &html)
{
    QString webChannelCode = QStringLiteral(R"(<script type="text/javascript" src="qrc:///qtwebchannel/qwebchannel.js"></script>)");
    webChannelCode += QStringLiteral(
        "<script> document.addEventListener(\"DOMContentLoaded\", function() {"
        "new QWebChannel(qt.webChannelTransport, function (channel) {"
        "Marble = channel.objects.Marble;"
        "});"
        "}); </script>"
        "</head>");
    html.replace(QStringLiteral("</head>"), webChannelCode);
}

void MarbleLegendBrowser::reverseSupportCheckboxes(QString &html)
{
    const QString old = QStringLiteral("<a href=\"checkbox:cities\"/>");

    QString checked;
    if (d->m_checkBoxMap[QStringLiteral("cities")])
        checked = QStringLiteral("checked");

    const QString repair = QLatin1StringView(
                               "<input style=\"position: relative; top: -4px;\" type=\"checkbox\" "
                               "onchange=\"Marble.setCheckedProperty(this.name, this.checked);\" ")
        + checked + QLatin1StringView(" name=\"cities\"/>");

    html.replace(old, repair);
}

QString MarbleLegendBrowser::generateSectionsHtml()
{
    // Generate HTML to include into legend.html here.

    QString customLegendString;

    if (d->m_marbleModel == nullptr || d->m_marbleModel->mapTheme() == nullptr)
        return {};

    const GeoSceneDocument *currentMapTheme = d->m_marbleModel->mapTheme();

    d->m_symbolMap.clear();

    /* Okay, if you are reading it now, be ready for hell!
     * We can't optimize this part of Legend Browser, but we will
     * do it, anyway. It's complicated a lot, the most important
     * thing is to understand everything.
     */
    for (const GeoSceneSection *section : currentMapTheme->legend()->sections()) {
        // Each section is divided into the "well"
        // Well is like a block of data with rounded corners
        customLegendString += QLatin1StringView("<div class=\"well well-small well-legend\">");

        const QString heading = QCoreApplication::translate("DGML", section->heading().toUtf8().constData());
        QString checkBoxString;
        if (section->checkable()) {
            // If it's needed to make a checkbox here, we will
            QString const checked = d->m_checkBoxMap[section->connectTo()] ? QStringLiteral("checked") : QString();
            /* Important comment:
             * We inject Marble object into JavaScript of each legend html file
             * This is only one way to handle checkbox changes we see, so
             * Marble.setCheckedProperty is a function that does it
             */
            if (!section->radio().isEmpty()) {
                checkBoxString = QLatin1StringView(
                                     "<label class=\"section-head\">"
                                     "<input style=\"position: relative; top: -4px;\" type=\"radio\" "
                                     "onchange=\"Marble.setRadioCheckedProperty(this.value, this.name ,this.checked);\" ")
                    + checked + QLatin1StringView(" value=\"") + section->connectTo() + QLatin1StringView("\" name=\"") + section->radio()
                    + QLatin1StringView("\" /><span>") + heading + QLatin1StringView("</span></label>");

            } else {
                checkBoxString = QLatin1StringView(
                                     "<label class=\"section-head\">"
                                     "<input style=\"position: relative; top: -4px;\" type=\"checkbox\" "
                                     "onchange=\"Marble.setCheckedProperty(this.name, this.checked);\" ")
                    + checked + QLatin1StringView(" name=\"") + section->connectTo() + QLatin1StringView("\" /><span>") + heading
                    + QLatin1StringView("</span></label>");
            }
            customLegendString += checkBoxString;

        } else {
            customLegendString += QLatin1StringView("<h4 class=\"section-head\">") + heading + QLatin1StringView("</h4>");
        }

        for (const GeoSceneItem *item : section->items()) {
            // checkbox for item
            QString checkBoxString;
            if (item->checkable()) {
                QString const checked = d->m_checkBoxMap[item->connectTo()] ? QStringLiteral("checked") : QString();
                checkBoxString = QLatin1StringView(
                                     "<input type=\"checkbox\" "
                                     "onchange=\"Marble.setCheckedProperty(this.name, this.checked);\" ")
                    + checked + QLatin1StringView(" name=\"") + item->connectTo() + QLatin1StringView("\" />");
            }

            // pixmap and text
            QString src;
            QString styleDiv;
            int pixmapWidth = 24;
            int pixmapHeight = 12;
            if (!item->icon()->pixmap().isEmpty()) {
                QString path = MarbleDirs::path(item->icon()->pixmap());
                const QPixmap oncePixmap(path);
                pixmapWidth = oncePixmap.width();
                pixmapHeight = oncePixmap.height();
                src = QUrl::fromLocalFile(path).toString();
                styleDiv = QLatin1StringView("width: ") + QString::number(pixmapWidth) + QLatin1StringView("px; height: ") + QString::number(pixmapHeight)
                    + QLatin1StringView("px;");
            } else {
                // Workaround for rendered border around empty images in webkit
                src = QStringLiteral("data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7");
            }
            // NOTICE. There are some pixmaps without image, so we should
            //         create just a plain rectangle with set color
            if (QColor(item->icon()->color()).isValid()) {
                const QColor color = item->icon()->color();
                styleDiv = QLatin1StringView("width: ") + QString::number(pixmapWidth) + QLatin1StringView("px; height: ") + QString::number(pixmapHeight)
                    + QLatin1StringView("px; background-color: ") + color.name() + QLatin1Char(';');
            }
            styleDiv += QStringLiteral(" position: relative; top: -3px;");
            const QString text = QCoreApplication::translate("DGML", item->text().toUtf8().constData());
            QString html = QLatin1StringView(
                               "<div class=\"legend-entry\">"
                               "  <label>")
                + checkBoxString + QLatin1StringView(R"(    <img class="image-pic" src=")") + src + QLatin1StringView("\" style=\"") + styleDiv
                + QLatin1StringView(
                               "\"/>"
                               "    <span class=\"kotation\" >")
                + text
                + QLatin1StringView(
                               "</span>"
                               "  </label>"
                               "</div>");
            customLegendString += html;
        }
        customLegendString += QLatin1StringView("</div>"); // <div class="well">
    }

    return customLegendString;
}

void MarbleLegendBrowser::setCheckedProperty(const QString &name, bool checked)
{
    if (checked != d->m_checkBoxMap[name]) {
        d->m_checkBoxMap[name] = checked;
        Q_EMIT toggledShowProperty(name, checked);
    }
}

void MarbleLegendBrowser::setRadioCheckedProperty(const QString &value, const QString &name, bool checked)
{
    Q_UNUSED(value)
    if (checked != d->m_checkBoxMap[name]) {
        d->m_checkBoxMap[name] = checked;
        Q_EMIT toggledShowProperty(name, checked);
    }
}

}

#include "moc_MarbleLegendBrowser.cpp"
