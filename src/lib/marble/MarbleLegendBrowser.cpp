//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn      <tackat@kde.org>
// Copyright 2007      Inge Wallin       <ingwa@kde.org>
// Copyright 2012      Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
// Copyright 2013      Yazeed Zoabi      <yazeedz.zoabi@gmail.com>
//


#include "MarbleLegendBrowser.h"

#include <QCoreApplication>
#include <QUrl>
#include <QDesktopServices>
#include <QEvent>
#include <QFile>
#include <QMouseEvent>
#include <QPainter>
#include <QRegExp>

#ifndef MARBLE_NO_WEBKITWIDGETS
#include <QWebEnginePage>
#include <QWebChannel>
#include "MarbleWebView.h"
#endif

#include <QTextDocument>

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneLegend.h"
#include "GeoSceneSection.h"
#include "GeoSceneIcon.h"
#include "GeoSceneItem.h"
#include "GeoSceneProperty.h"
#include "GeoSceneSettings.h"
#include "MarbleModel.h"
#include "MarbleDebug.h"
#include "TemplateDocument.h"
#include "MarbleDirs.h"

namespace Marble
{

class MarbleLegendBrowserPrivate
{
 public:
    MarbleModel            *m_marbleModel;
    QMap<QString, bool>     m_checkBoxMap;
    QMap<QString, QPixmap>  m_symbolMap;
    QString                 m_currentThemeId;
    MarbleJsWrapper        *m_jsWrapper;
};


// ================================================================


MarbleLegendBrowser::MarbleLegendBrowser( QWidget *parent )
    : MarbleWebView( parent ),
      d( new MarbleLegendBrowserPrivate )
{
    d->m_marbleModel = nullptr;
    d->m_jsWrapper = new MarbleJsWrapper(this);
}

MarbleLegendBrowser::~MarbleLegendBrowser()
{
    delete d;
}

void MarbleLegendBrowser::setMarbleModel( MarbleModel *marbleModel )
{
    // We need this to be able to get to the MapTheme.
    d->m_marbleModel = marbleModel;

    if ( d->m_marbleModel ) {
        connect ( d->m_marbleModel, SIGNAL(themeChanged(QString)),
                  this, SLOT(initTheme()) );
    }
}

QSize MarbleLegendBrowser::sizeHint() const
{
    return QSize( 320, 320 );
}

void MarbleLegendBrowser::initTheme()
{
    // Check for a theme specific legend.html first
    if ( d->m_marbleModel != nullptr && d->m_marbleModel->mapTheme() != nullptr )
    {
        const GeoSceneDocument *currentMapTheme = d->m_marbleModel->mapTheme();

        d->m_checkBoxMap.clear();

        for ( const GeoSceneProperty *property: currentMapTheme->settings()->allProperties() ) {
            if ( property->available() ) {
                d->m_checkBoxMap[ property->name() ] = property->value();
            }
        }

        disconnect ( currentMapTheme, SIGNAL(valueChanged(QString,bool)), nullptr, nullptr );
        connect ( currentMapTheme, SIGNAL(valueChanged(QString,bool)),
                  this, SLOT(setCheckedProperty(QString,bool)) );
    }

    if ( isVisible() ) {
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
    if (d->m_marbleModel->mapTheme() != nullptr ) {
        const GeoSceneDocument *currentMapTheme = d->m_marbleModel->mapTheme();

        legendPath = MarbleDirs::path(QLatin1String("maps/") +
            currentMapTheme->head()->target() + QLatin1Char('/') +
            currentMapTheme->head()->theme() + QLatin1String("/legend.html"));
    }
    if ( legendPath.isEmpty() ) {
        legendPath = MarbleDirs::path(QStringLiteral("legend.html"));
    }

    QString finalHtml = readHtml( QUrl::fromLocalFile( legendPath ) );

    TemplateDocument doc(finalHtml);
    finalHtml = doc.finalText();

    injectWebChannel(finalHtml);
    reverseSupportCheckboxes(finalHtml);

    // Generate some parts of the html from the MapTheme <Legend> tag.
    const QString sectionsHtml = generateSectionsHtml();

    // And then create the final html from these two parts.
    finalHtml.replace( QString( "<!-- ##customLegendEntries:all## -->" ), sectionsHtml );

    translateHtml( finalHtml );

    QUrl baseUrl = QUrl::fromLocalFile( legendPath );

    // Set the html string in the QTextBrowser.
    MarbleWebPage * page = new MarbleWebPage(this);
    connect( page, SIGNAL(linkClicked(QUrl)), this, SLOT(openLinkExternally(QUrl)) );
    page->setHtml(finalHtml, baseUrl);
    setPage(page);

    QWebChannel *channel = new QWebChannel(page);
    channel->registerObject(QStringLiteral("Marble"), d->m_jsWrapper);
    page->setWebChannel(channel);

    if ( d->m_marbleModel ) {
        page->toHtml([=]( QString document ) {
            d->m_marbleModel->setLegend( new QTextDocument(document) );
        });
    }
#endif
}

void MarbleLegendBrowser::openLinkExternally( const QUrl &url )
{
    if (url.scheme() == QLatin1String("tour")) {
        emit tourLinkClicked(QLatin1String("maps/") + url.host() + url.path());
    } else {
        QDesktopServices::openUrl( url );
    }
}

bool MarbleLegendBrowser::event( QEvent * event )
{
    // "Delayed initialization": legend gets created only 
    if ( event->type() == QEvent::Show ) {
        setVisible(true);
        loadLegend();
        return true;
    }

    return MarbleWebView::event( event );
}

QString MarbleLegendBrowser::readHtml( const QUrl & name )
{
    QString html;

    QFile data( name.toLocalFile() );
    if ( data.open( QFile::ReadOnly ) ) {
        QTextStream in( &data );
        html = in.readAll();
        data.close();
    }

    return html;
}

void MarbleLegendBrowser::translateHtml( QString & html )
{
    // must match string extraction in Messages.sh
    QString s = html;
    QRegExp rx( "</?\\w+((\\s+\\w+(\\s*=\\s*(?:\".*\"|'.*'|[^'\">\\s]+))?)+\\s*|\\s*)/?>" );
    rx.setMinimal( true );
    s.replace( rx, "\n" );
    s.replace( QRegExp( "\\s*\n\\s*" ), "\n" );
    const QStringList words = s.split(QLatin1Char('\n'), QString::SkipEmptyParts);

    QStringList::const_iterator i = words.constBegin();
    QStringList::const_iterator const end = words.constEnd();
    for (; i != end; ++i )
        html.replace(*i, QCoreApplication::translate("Legends", (*i).toUtf8().constData()));
}

void MarbleLegendBrowser::injectWebChannel(QString &html)
{
  QString webChannelCode = "<script type=\"text/javascript\" src=\"qrc:///qtwebchannel/qwebchannel.js\"></script>";
  webChannelCode += "<script> document.addEventListener(\"DOMContentLoaded\", function() {"
                      "new QWebChannel(qt.webChannelTransport, function (channel) {"
                      "Marble = channel.objects.Marble;"
                      "});"
                     "}); </script>"
                     "</head>";
  html.replace("</head>", webChannelCode);
}

void MarbleLegendBrowser::reverseSupportCheckboxes(QString &html)
{
    const QString old = "<a href=\"checkbox:cities\"/>";

    QString checked;
    if (d->m_checkBoxMap["cities"])
        checked = "checked";

    const QString repair = QLatin1String(
            "<input style=\"position: relative; top: -4px;\" type=\"checkbox\" "
            "onchange=\"Marble.setCheckedProperty(this.name, this.checked);\" ") + checked + QLatin1String(" name=\"cities\"/>");

    html.replace(old, repair);
}

QString MarbleLegendBrowser::generateSectionsHtml()
{
    // Generate HTML to include into legend.html here.

    QString customLegendString;

    if ( d->m_marbleModel == nullptr || d->m_marbleModel->mapTheme() == nullptr )
        return QString();

    const GeoSceneDocument *currentMapTheme = d->m_marbleModel->mapTheme();

    d->m_symbolMap.clear();

    /* Okay, if you are reading it now, be ready for hell!
     * We can't optimize this part of Legend Browser, but we will
     * do it, anyway. It's complicated a lot, the most important
     * thing is to understand everything.
     */
    for ( const GeoSceneSection *section: currentMapTheme->legend()->sections() ) {
        // Each section is divided into the "well"
        // Well is like a block of data with rounded corners
        customLegendString += QLatin1String("<div class=\"well well-small well-legend\">");

        const QString heading = QCoreApplication::translate("DGML", section->heading().toUtf8().constData());
        QString checkBoxString;
        if (section->checkable()) {
            // If it's needed to make a checkbox here, we will
            QString const checked = d->m_checkBoxMap[section->connectTo()] ? "checked" : "";
            /* Important comment:
             * We inject Marble object into JavaScript of each legend html file
             * This is only one way to handle checkbox changes we see, so
             * Marble.setCheckedProperty is a function that does it
             */
            if(!section->radio().isEmpty()) {
                checkBoxString = QLatin1String(
                        "<label class=\"section-head\">"
                        "<input style=\"position: relative; top: -4px;\" type=\"radio\" "
                        "onchange=\"Marble.setRadioCheckedProperty(this.value, this.name ,this.checked);\" ") +
                        checked + QLatin1String(" value=\"") + section->connectTo() + QLatin1String("\" name=\"") + section->radio() + QLatin1String("\" /><span>")
                        + heading +
                        QLatin1String("</span></label>");

            } else {
                checkBoxString = QLatin1String(
                        "<label class=\"section-head\">"
                        "<input style=\"position: relative; top: -4px;\" type=\"checkbox\" "
                        "onchange=\"Marble.setCheckedProperty(this.name, this.checked);\" ") + checked + QLatin1String(" name=\"") + section->connectTo() + QLatin1String("\" /><span>")
                        + heading +
                        QLatin1String("</span></label>");

            }
            customLegendString += checkBoxString;

        } else {
            customLegendString += QLatin1String("<h4 class=\"section-head\">") + heading + QLatin1String("</h4>");
        }

        for (const GeoSceneItem *item: section->items()) {

            // checkbox for item
            QString checkBoxString;
            if (item->checkable()) {
                QString const checked = d->m_checkBoxMap[item->connectTo()] ? "checked" : "";
                checkBoxString = QLatin1String(
                        "<input type=\"checkbox\" "
                        "onchange=\"Marble.setCheckedProperty(this.name, this.checked);\" ")
                        + checked + QLatin1String(" name=\"") + item->connectTo() + QLatin1String("\" />");

            }

            // pixmap and text
            QString src;
            QString styleDiv;
            int pixmapWidth = 24;
            int pixmapHeight = 12;
            if (!item->icon()->pixmap().isEmpty()) {
                QString path = MarbleDirs::path( item->icon()->pixmap() );
                const QPixmap oncePixmap(path);
                pixmapWidth = oncePixmap.width();
                pixmapHeight = oncePixmap.height();
                src = QUrl::fromLocalFile( path ).toString();
                styleDiv = QLatin1String("width: ") + QString::number(pixmapWidth) + QLatin1String("px; height: ") +
                        QString::number(pixmapHeight) + QLatin1String("px;");
            } else {
              // Workaround for rendered border around empty images in webkit
              src = "data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7";
            }
            // NOTICE. There are some pixmaps without image, so we should
            //         create just a plain rectangle with set color
            if (QColor(item->icon()->color()).isValid()) {
                const QColor color = item->icon()->color();
                styleDiv = QLatin1String("width: ") + QString::number(pixmapWidth) + QLatin1String("px; height: ") +
                        QString::number(pixmapHeight) + QLatin1String("px; background-color: ") + color.name() + QLatin1Char(';');
            }
            styleDiv += " position: relative; top: -3px;";
            const QString text = QCoreApplication::translate("DGML", item->text().toUtf8().constData());
            QString html = QLatin1String(
                    "<div class=\"legend-entry\">"
                    "  <label>") + checkBoxString + QLatin1String(
                    "    <img class=\"image-pic\" src=\"") + src + QLatin1String("\" style=\"") + styleDiv + QLatin1String("\"/>"
                    "    <span class=\"kotation\" >") + text + QLatin1String("</span>"
                    "  </label>"
                    "</div>");
            customLegendString += html;
        }
        customLegendString += QLatin1String("</div>"); // <div class="well">
    }

    return customLegendString;
}

void MarbleLegendBrowser::setCheckedProperty( const QString& name, bool checked )
{
    if (checked != d->m_checkBoxMap[name]) {
        d->m_checkBoxMap[name] = checked;
        emit toggledShowProperty( name, checked );
    }
}

void MarbleLegendBrowser::setRadioCheckedProperty( const QString& value, const QString& name , bool checked )
{
  Q_UNUSED(value)
  if (checked != d->m_checkBoxMap[name]) {
      d->m_checkBoxMap[name] = checked;
      emit toggledShowProperty( name, checked );
  }
}

}

#include "moc_MarbleLegendBrowser.cpp"
