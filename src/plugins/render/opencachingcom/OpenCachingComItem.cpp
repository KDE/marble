//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Anders Lund <anders@alweb.dk>
//

#include "OpenCachingComItem.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "MarbleModel.h"
#include "PositionTracking.h"
#include "MarbleMath.h"
#include "MarbleLocale.h"
#include "Planet.h"

#include "ui_CacheDetailsDialog.h"
#include "OpenCachingComModel.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPixmap>
#include <QAction>
#include <QDialog>
#include <QDateTime>
#include <QFile>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>

#include <QDebug>

namespace Marble
{

OpenCachingComItem::OpenCachingComItem( QVariantMap cache, QObject *parent )
    : AbstractDataPluginItem( parent )
     , m_ui( 0 )
     , m_model( qobject_cast<OpenCachingComModel*>(parent) )
     , m_cache( cache )
     , m_action( new QAction( this ) )
{
    setId( cache["oxcode"].toString() );
    setCoordinate( GeoDataCoordinates( cache["location"].toMap()["lon"].toReal(), cache["location"].toMap()["lat"].toReal(), 0.0, GeoDataCoordinates::Degree ) );
    setTarget( "earth" );
    updateTooltip();
    m_action->setText( m_cache["name"].toString() );

    connect( action(), SIGNAL( triggered() ),
             this, SLOT( showInfoDialog() ) );

    setSize(QSizeF(24,24));
}

OpenCachingComItem::~OpenCachingComItem()
{
}

QString OpenCachingComItem::itemType() const
{
    return "OpenCachingComItem";
}

bool OpenCachingComItem::initialized()
{
    return !id().isEmpty();
}

QAction *OpenCachingComItem::action()
{
    return m_action;
}

void OpenCachingComItem::showInfoDialog()
{
    QDialog dialog;

    Ui::CacheDetailsDialog ui;
    ui.setupUi( &dialog );

    m_ui = &ui;

    dialog.setWindowTitle( tr("Cache Details for") + " " + id() );

    m_ui->labelIcon->setPixmap(QPixmap(":/" + iconName() + ".png"));
    m_ui->labelHeader->setText(QString("<big><strong>" + m_cache["name"].toString() + "</strong></big>"));

    // basic details
    QString details = "<table><tr><td colspan=\"2\" valign=\"middle\">"
        + tr(m_cache["type"].toString().toUtf8())
        + "<br/>" + id() + "<br/>" + coordinate().toString() + "</td></tr>"
        + "<tr><td align=\"right\">" + tr("Size:") + " </td><td>" + ratingNumberString(m_cache["size"]) + "</td></tr>"
        + "<tr><td align=\"right\">" + tr("Difficulty:") + " </td><td>" + ratingNumberString(m_cache["difficulty"]) + "</td></tr>"
        + "<tr><td align=\"right\">" + tr("Terrain:") + " </td><td>" + ratingNumberString(m_cache["terrain"]) + "</td></tr>"
        + "<tr><td align=\"right\">" + tr("Awsomeness:") + " </td><td>" + ratingNumberString(m_cache["awsomeness"]) + "</td></tr>"
        + "<tr><td align=\"right\">" + tr("Hidden by:") + " </td><td>" + m_cache["hidden_by"].toMap()["name"].toString() + "</td></tr>"
        + "<tr><td align=\"right\">" + tr("Hidden:") + " </td><td>" + dateString(m_cache["hidden"]) + "</td></tr>"
        + "<tr><td align=\"right\">" + tr("Last found:") + " </td><td>" + dateString(m_cache["last_found"]) + "</td></tr>";

    // list of tags
    // ### cache the translated cache string. Nessecary??
    if (m_cache["strTags"].toString().isEmpty() )
    {
        QStringList tags = m_cache["tags"].toStringList();
        QString strTags;
        for (int i=0; i<tags.count();i++)
        {
            if (i)
            {
                strTags += ", ";
            }
            strTags += tr(tags[i].toUtf8());
        }
        m_cache["strTags"] = strTags;
    }
    QString strTags = m_cache["strTags"].toString();
    if (!strTags.isEmpty())
    {
        details += "<tr><td align=\"right\">" + tr("Tags:") + " </td><td>" + strTags + "</td></tr>";
    }

    // "series" - name of a series that this cache belongs to
    QString series = m_cache["series"].toString();
    if (!series.isEmpty())
    {
        details += "<tr><td align=\"right\">" + tr("Series:") + " </td><td>" + series + "</td></tr>";
    }

    // verification options translated
    // these are ways to verify a visit to the cache
    QVariantMap verifyOpts = m_cache["verification"].toMap();
    if (verifyOpts.count())
    {
        QStringList verifyOptsStrings;
        if (verifyOpts["number"].isValid())
            verifyOptsStrings << tr("number");
        if (verifyOpts["chirp"].isValid())
            verifyOptsStrings << tr("chirp");
        if (verifyOpts["qr"].isValid())
            verifyOptsStrings << tr("qr code");
        if (verifyOpts["code_phrase"].isValid())
            verifyOptsStrings << tr("code phrase");

        details += "<tr><td align=\"right\">" + tr("Verification:") + " </td><td>" + verifyOptsStrings.join(", ") + "</td></tr>";
    }

    // show a distance to the cache, either from current position if we know,
    // or from the "home" position
    bool hasposition = false;
    PositionTracking *tracking = m_model->marbleModel()->positionTracking();
    if (tracking)
    {
        GeoDataCoordinates location = tracking->currentLocation();
        if (location.isValid())
        {
            QString dist = formatDistance( distanceSphere(location, coordinate()) );
            details += "<tr><td align=\"right\">" + tr("Distance:") + " </td><td>" + dist + "</td></tr>";
            hasposition = true;
        }
    }
    if (!hasposition)
    {
        GeoDataCoordinates homelocation = m_model->home();
        if ( homelocation.isValid() )
        {
            QString dist = formatDistance( distanceSphere(homelocation, coordinate()) );
            details += "<tr><td align=\"right\">" + tr("Distance from home:") + " </td><td>" + dist + "</td></tr>";
        }
    }

    ui.labelGeneral->setText(details.append("</table>"));

    // tags represented as icons
    int numtags = m_cache["tags"].toList().count();
    if (numtags)
    {
        QStringList tags = m_cache["tags"].toStringList();
        int tagrows = (numtags/6) + 1;
        QString tagiconshtml;
        for (int i=0; i<numtags;i++)
        {
            tagiconshtml += QString("<img src=\":/%1.png\" alt=\"%2\">")
                .arg(tags.at(i).toLower().replace(" ","-")).arg(tags.at(i));
            if (0 == (i+1)%tagrows)
            {
                tagiconshtml += "<br/>";
            }
        }
        ui.labelTagIcons->setText(tagiconshtml);
    }

    // Description tab
    QString description = m_cache["description"].toString();
    if ( description.isEmpty() )
    {
        QString url("http://www.opencaching.com/api/geocache/" + id() + "?Authorization=" + AUTHKEY);
        m_model->fetchData( url, "description_and_logs", this );
    } else {
        fillDialogTabs();
    }

    connect( ui.buttonClose, SIGNAL(clicked()), &dialog, SLOT(close()) );
    connect( ui.buttonClose, SIGNAL(clicked()), this, SLOT(dialogCloseClicked()) );

    dialog.exec();
}

void OpenCachingComItem::paint( QPainter *painter )
{
    QPixmap pm(":/" + iconName() + "-m.png");
    painter->drawPixmap( 0, 0, pm );
}

void OpenCachingComItem::updateTooltip()
{
    if ( !id().isEmpty() ) {
        // ### how do i update the tooltip on hover?
//         QString strDist;
//         bool hasposition = false;
//         PositionTracking *tracking = m_model->marbleModel()->positionTracking();
//         if (tracking)
//         {
//             GeoDataCoordinates location = tracking->currentLocation();
//             if (location.isValid())
//             {
//                 QString dist = formatDistance( distanceSphere(location, coordinate()) );
//                 strDist = tr("Distance: ") + dist;
//                 hasposition = true;
//             }
//         }
//         if (!hasposition)
//         {
//             GeoDataCoordinates homelocation = m_model->home();
//             if ( homelocation.isValid() )
//             {
//                 QString dist = formatDistance( distanceSphere(homelocation, coordinate()) );
//                 strDist = tr("Distance from home: ") + dist;
//             }
//         }

        QString html = "<table cellpadding=\"2\">";
        html += "<tr><td colspan=\"2\"><table cellpadding=\"3\"><tr><td><img src=\":/" + iconName() + "\"></td>"
            + "<td valign=\"middle\"><big><strong>" + m_cache["name"].toString() + "</strong></big></td></tr></table></td></tr>"
            + "<tr><td align=\"left\">"
            + tr(m_cache["type"].toString().toUtf8())
            + "<br/>" + tr( "by <em>" ) + m_cache["hidden_by"].toMap()["name"].toString() + "</em>"
            + "<br/>" + coordinate().lonToString()
            + "<br/>" + coordinate().latToString()
            + "</td><td align=\"right\">"
            + tr( "Difficulty") + ": <span style=\"background:#1e4381;color:white;\">&nbsp;" + ratingNumberString(m_cache["difficulty"]) + "&nbsp;</span>"
            + "<br/>" + tr( "Terrain" ) + ": <span style=\"background:#dda24d;color:white;\">&nbsp;" + ratingNumberString(m_cache["terrain"]) + "&nbsp;</span>"
            + "<br/>" + tr( "Awsomeness" ) + ": <span style=\"background:#e27c31;color:white;\">&nbsp;" + ratingNumberString(m_cache["awsomeness"]) + "&nbsp;</span>"
            + "<br/>" + tr( "Size" ) + ": <span style=\"background:#807f39;color:white;\">&nbsp;" + ratingNumberString(m_cache["size"]) + "&nbsp;</span></td></tr>"
            + "<tr><td colspan=\"2\">" + tr("Last found: ") + dateString(m_cache["last_found"]) + "</td></tr>"
//             + "<tr><td colspan=\"2\">" + strDist + "</td></tr>"
            + "</table>";
        setToolTip( html );
    }
}

bool OpenCachingComItem::operator<( const AbstractDataPluginItem *other ) const
{
    return id() < other->id();
}

void OpenCachingComItem::addDownloadedFile( const QString &url, const QString &type )
{
    if (type == "description_and_logs")
    {
        QFile file( url );
        if (! file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            return;
        }

        QScriptEngine engine;

        // Qt requires parentheses around json code
        QScriptValue data = engine.evaluate( "(" + QString::fromUtf8( file.readAll() ) + ")" );
        QVariantMap cache = data.toVariant().toMap();

        m_cache["description"] = cache["description"];
        m_cache["logs"] = cache["logs"];
        m_cache["hint"] = cache["hint"];

        fillDialogTabs();
    }

    // ### images does not work, opencaching.com redirects them to
    // a localized location and can not return them.
    else if (type.startsWith("image"))
    {
        int index = type.mid(6).toInt();
        m_images << url;
        if (m_ui)
        {
            m_ui->textDescription->append(
                "<p><img src=\"" + url + " width=\"100%\" height=\"auto\"/><br/>"
                + m_cache["images"].toList().at(index).toMap()["name"].toString()
                + "</p>");
        }
    }
}

void OpenCachingComItem::dialogCloseClicked()
{
    m_ui = 0;
}

QString OpenCachingComItem::dateString(const QVariant& ms)
{
    QString result;
    qlonglong t = ms.toLongLong();
    if (t)
    {
        result = QDateTime::fromMSecsSinceEpoch(t).toString(Qt::SystemLocaleShortDate);
    }
    else
    {
        result = "-";
    }
    return result;
}

QString OpenCachingComItem::iconName() const
{
    static QStringList iconnames;
    iconnames<<"traditional"<<"multi-cache"<<"puzzle"<<"virtual";

    QString res = m_cache["type"].toString().section(" ", 0, 0).toLower();
    if ( iconnames.contains(res) )
    {
        return res;
    }
    return "unknown";
}

const QString OpenCachingComItem::ratingNumberString(QVariant number) const
{
    return QString::number(number.toDouble(), 'f', 1);
}

QString OpenCachingComItem::formatDistance(qreal spheredistance) const
{
    qreal distance = m_model->marbleModel()->planet()->radius() * spheredistance;
    QString distanceString;
    QLocale::MeasurementSystem measurementSystem;
    measurementSystem = MarbleGlobal::getInstance()->locale()->measurementSystem();

    if ( measurementSystem == QLocale::MetricSystem ) {
        if ( distance >= 1000.0 ) {
            distanceString = tr("%1 km").arg( distance / 1000.0, 0, 'f', 2 );
        }
        else {
            distanceString = tr("%1 m").arg( distance, 0, 'f', 2 );
        }
    }
    else {
        distanceString = QString("%1 mi").arg( distance / 1000.0 * KM2MI, 0, 'f', 2 );
    }

    return distanceString;
}

void OpenCachingComItem::fillDialogTabs()
{
    if (m_ui)
    {
        m_ui->textDescription->setText(m_cache["description"].toString());

        // images
        // ### does not work at this point, talk to opencaching.com people!
        QVariantList images = m_cache["images"].toList();
        for (int i = 0; i < images.size(); i++)
        {
            if (m_images.size() > i)
            {
                // ### what about spoiler images? (don't display, but then what?)
                m_ui->textDescription->append(
                    "<p><img src=\"" + m_images.at(i) + " width=\"100%\" height=\"auto\"/><br/>"
                    + m_cache["images"].toList().at(i).toMap()["name"].toString()
                    + "</p>");
            }
            else
            {
                QVariantMap image = images.at(i).toMap();
                QString url =  "http://www.opencaching.com/api/" + id() + "/" + image["caption"].toString();
//                     qDebug()<<"Adding image: "<<url;
                m_model->fetchData(url, QString("image-%1").arg(i), this);
            }
        }

        QString hint = m_cache["hint"].toString().trimmed();
        if (! hint.isEmpty())
        {
            m_ui->textHint->setText(hint);
        }
        m_ui->tabWidget->setTabEnabled(2, !hint.isEmpty());

        QVariantList logs = m_cache["logs"].toList();
        QString logtext;
        for (int i = 0; i < logs.size(); ++i)
        {
            QVariantMap log = logs.at(i).toMap();
            if ( i )
            {
                logtext += "<hr>";
            }
            logtext += tr("User: ") + log["user"].toMap()["name"].toString() + "<br/>";
            logtext += tr("Type: ") + log["type"].toString() + "<br/>";
            logtext += tr("Date: ") + dateString(log["log_time"]) + "<br/><br/>";
            logtext += log["comment"].toString() + "<br/>";

            // TODO images
        }
        m_ui->textLogs->setText(logtext);
    }
}

} // namespace Marble

#include "OpenCachingComItem.moc"
