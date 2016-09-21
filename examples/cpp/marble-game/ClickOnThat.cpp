//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

// Self
#include "ClickOnThat.h"

// Qt
#include <QTime>
#include <QVector>
#include <QVariant>
#include <QStringList>
#include <QDir>

// Marble
#include <marble/MarbleWidget.h>
#include <marble/MarbleMap.h>
#include <marble/MarbleModel.h>
#include <marble/GeoDataTreeModel.h>
#include <marble/MarbleDirs.h>
#include <marble/MarblePlacemarkModel.h>

#include <marble/GeoDataDocument.h>
#include <marble/GeoDataPlacemark.h>
#include <marble/GeoDataStyle.h>
#include <marble/GeoDataStyleMap.h>
#include <marble/GeoDataIconStyle.h>
#include <marble/GeoDataLinearRing.h>
#include <marble/GeoDataPoint.h>
#include <marble/GeoDataPolygon.h>
#include <marble/GeoDataMultiGeometry.h>

#include <marble/GeoDataTypes.h>

namespace Marble
{
class ClickOnThatPrivate
{
public:
    ClickOnThatPrivate( MarbleWidget *marbleWidget )
    : m_marbleWidget( marbleWidget ),
      m_parent( 0 ),
      m_correctAnswerPlacemark( 0 ),
      m_selectPinDocument( 0 ),
      m_countryNames( 0 ),
      m_countryBoundaries( 0 )
      {
        m_continentsAndOceans
            << QStringLiteral("Asia") << QStringLiteral("Africa")
            << QStringLiteral("North America") << QStringLiteral("South America")
            << QStringLiteral("Antarctica") << QStringLiteral("Europe")
            << QStringLiteral("Australia")
            << QStringLiteral("Arctic Ocean") << QStringLiteral("Indian Ocean")
            << QStringLiteral("North Atlantic Ocean") << QStringLiteral("North Pacific Ocean")
            << QStringLiteral("South Pacific Ocean") << QStringLiteral("South Atlantic Ocean")
            << QStringLiteral("Southern Ocean");
      }

    ~ClickOnThatPrivate()
    {
        delete m_selectPinDocument;
    }

        MarbleWidget *m_marbleWidget;
        ClickOnThat *m_parent;

        /**
         * Store the GeoDataPlacemark also
         * for the correct answer so that
         * we can highlight and zoom in
         * ( to fit in the current view port )
         * to this placemark when user
         * choses to view the right answer.
         */
        GeoDataPlacemark *m_correctAnswerPlacemark;
        GeoDataCoordinates m_correctAnswer;

        /**
         * @p m_selectPinDocument shows a pin
         * on map indicating whether the user
         * has clicked on right country
         */
        GeoDataDocument *m_selectPinDocument;

        /**
        * Document to store point placemarks which
        * have country names ( from file "boundaryplacemarks.cache" )
        */
        GeoDataDocument *m_countryNames;

        /**
        * Document which have placemarks whose geometry
        * specifies the boundaries of a country
        * (from file "ne_50m_admin_0_countries.pn2" )
        */
        GeoDataDocument *m_countryBoundaries;


        /*
        * If the placemark used for posting question
        * represent a continent, the user needs to click exactly on
        * a particular country whose geometry contains this point
        * placemark on map ( Ideally it should be any country
        * within that continent ). Also, oceans are point placemark, so
        * user needs to click exactly on same point this placemark
        * represents ( Ideally it should anywhere within the ocean territory ).
        * So, to avoid such placemark we will use this list.
        */
        QStringList m_continentsAndOceans;
};

ClickOnThat::ClickOnThat( MarbleWidget *marbleWidget )
    : QObject(),
      d( new ClickOnThatPrivate(marbleWidget) )
{
    d->m_parent = this;
    connect( this, SIGNAL(announceHighlight(qreal,qreal,GeoDataCoordinates::Unit)),
             d->m_marbleWidget, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)) );
}

ClickOnThat::~ClickOnThat()
{
    delete d;
}

void ClickOnThat::disablePinDocument()
{
    if ( d->m_selectPinDocument ) {
        d->m_selectPinDocument->setVisible( false );
        d->m_marbleWidget->model()->treeModel()->updateFeature( d->m_selectPinDocument );
    }
}

void ClickOnThat::initiateGame()
{
    /**
     * First remove the GeoDataDocument, which displays
     * country names, from map.
     */
    if ( !d->m_countryNames ) {
        const GeoDataTreeModel *const treeModel = d->m_marbleWidget->model()->treeModel();
        for ( int i = 0; i < treeModel->rowCount(); ++i ) {
            QVariant const data = treeModel->data ( treeModel->index ( i, 0 ), MarblePlacemarkModel::ObjectPointerRole );
            GeoDataObject *object = qvariant_cast<GeoDataObject*>( data );
            Q_ASSERT_X( object, "CountryByShape::initiateGame",
                        "failed to get valid data from treeModel for GeoDataObject" );
            if ( object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
                GeoDataDocument *doc = static_cast<GeoDataDocument*>( object );
                QFileInfo fileInfo( doc->fileName() );
                if (fileInfo.fileName() == QLatin1String("boundaryplacemarks.cache")) {
                    d->m_countryNames = doc;
                    break;
                }
            }
        }
    }

    if ( !d->m_countryBoundaries ) {
        const GeoDataTreeModel *const treeModel = d->m_marbleWidget->model()->treeModel();
        for ( int i = 0; i < treeModel->rowCount(); ++i ) {
            QVariant const data = treeModel->data ( treeModel->index ( i, 0 ), MarblePlacemarkModel::ObjectPointerRole );
            GeoDataObject *object = qvariant_cast<GeoDataObject*>( data );
            Q_ASSERT_X( object, "MainWindow::initiateGame",
                        "failed to get valid data from treeModel for GeoDataObject" );
            if ( object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
                GeoDataDocument *const doc = static_cast<GeoDataDocument*>( object );
                QFileInfo fileInfo( doc->fileName() );
                if (fileInfo.fileName() == QLatin1String("ne_50m_admin_0_countries.pn2")) {
                    d->m_countryBoundaries = doc;
                    break;
                }
            }
        }
    }

    if ( !d->m_selectPinDocument ) {
        d->m_selectPinDocument = new GeoDataDocument;
        GeoDataPlacemark *pinPlacemark = new GeoDataPlacemark;

        GeoDataStyle::Ptr pinStyle(new GeoDataStyle);
        pinStyle->setId(QStringLiteral("answer"));
        GeoDataIconStyle iconStyle;
        iconStyle.setIconPath(MarbleDirs::path(QStringLiteral("bitmaps/target.png")));
        pinStyle->setIconStyle( iconStyle );

        GeoDataStyleMap styleMap;
        styleMap.setId(QStringLiteral("default-map"));
        styleMap.insert(QStringLiteral("normal"), QLatin1Char('#') + pinStyle->id());

        d->m_selectPinDocument->addStyle( pinStyle );
        d->m_selectPinDocument->addStyleMap( styleMap );

        d->m_selectPinDocument->append( pinPlacemark );
        pinPlacemark->setStyleUrl(QLatin1Char('#') + styleMap.id());
        d->m_selectPinDocument->setVisible( false );

        // Add this document to treeModel
        d->m_marbleWidget->model()->treeModel()->addDocument( d->m_selectPinDocument );
    }

    d->m_marbleWidget->setHighlightEnabled( true );
    d->m_marbleWidget->centerOn( 23.0, 42.0 );
    d->m_marbleWidget->setDistance( 7500 );
    connect( d->m_marbleWidget, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)),
             this, SLOT(determineResult(qreal,qreal,GeoDataCoordinates::Unit)) );

    if ( d->m_countryBoundaries &&
        d->m_countryNames )
    {
        d->m_countryNames->setVisible( false );
        d->m_marbleWidget->model()->treeModel()->updateFeature( d->m_countryNames );
        emit gameInitialized();
    }
}

void ClickOnThat::postQuestion( QObject *gameObject )
{
    /**
    * Find a random placemark
    */
    Q_ASSERT_X( d->m_countryNames, "ClickOnThat::postQuestion",
                "CountryByShapePrivate::m_countryNames is NULL" );
    QVector<GeoDataPlacemark*> countryPlacemarks = d->m_countryNames->placemarkList();

    uint randomSeed = uint(QTime::currentTime().msec());
    qsrand( randomSeed );

    GeoDataPlacemark *placemark = 0;
    GeoDataPoint *point = 0;
    bool found = false;
    while( !found ) {
        placemark = countryPlacemarks[qrand()%countryPlacemarks.size()];
        if ( !d->m_continentsAndOceans.contains(placemark->name(), Qt::CaseSensitive) ) {
            found = true;
            point = dynamic_cast<GeoDataPoint*>( placemark->geometry() );
        }
    }
    if ( point ) {
        d->m_correctAnswerPlacemark = placemark;
        d->m_correctAnswer = point->coordinates();
        if ( gameObject ) {
            QMetaObject::invokeMethod( gameObject, "clickOnThatQuestion",
                                    Q_ARG(QVariant, QVariant(placemark->name())) );
        }
    }
}

void ClickOnThat::updateSelectPin(bool result, const GeoDataCoordinates &clickedPoint )
{
    QDir dir;
    QString iconPath = dir.absolutePath();
    if ( result ) {
        //iconPath = MarbleDirs::path("bitmaps/MapTackRoundHeadGreen.png");
        iconPath += QLatin1String("/MapTackRoundHeadGreen.png");
    }
    else {
        iconPath += QLatin1String("/MapTackRoundHeadRed.png");
    }

    GeoDataStyle::Ptr style = d->m_selectPinDocument->style(QStringLiteral("answer"));
    style->iconStyle().setIconPath( iconPath );
    d->m_selectPinDocument->addStyle( style );

    QVector<GeoDataPlacemark*> placemarkList = d->m_selectPinDocument->placemarkList();
    if ( placemarkList.size() > 0 ) {
        placemarkList[0]->setCoordinate( clickedPoint );
    }

    if ( !d->m_selectPinDocument->isVisible() ) {
        d->m_selectPinDocument->setVisible( true );
    }
    d->m_marbleWidget->model()->treeModel()->updateFeature( d->m_selectPinDocument );
}

void ClickOnThat::determineResult( qreal lon, qreal lat, GeoDataCoordinates::Unit unit )
{
    GeoDataCoordinates coord( lon, lat, 0, unit );

    Q_ASSERT_X( d->m_countryNames, "ClickOnThat::determineResult",
                "CountryByShapePrivate::m_countryBoundaries is NULL" );
    QVector<GeoDataFeature*>::Iterator i = d->m_countryBoundaries->begin();
    QVector<GeoDataFeature*>::Iterator const end = d->m_countryBoundaries->end();

    bool foundStandardPoint = false;
    bool foundClickedPoint = false;
    for ( ; i != end; ++i ) {
        GeoDataPlacemark *country = static_cast<GeoDataPlacemark*>( *i );

        GeoDataPolygon *polygon = dynamic_cast<GeoDataPolygon*>( country->geometry() );
        GeoDataLinearRing *linearring = dynamic_cast<GeoDataLinearRing*>( country->geometry() );
        GeoDataMultiGeometry *multigeom = dynamic_cast<GeoDataMultiGeometry*>( country->geometry() );

        foundClickedPoint = false;
        foundStandardPoint = false;
        if ( polygon &&
            polygon->contains( coord ) &&
            polygon->contains(d->m_correctAnswer) )
        {
            foundClickedPoint = true;
            foundStandardPoint = true;
            d->m_correctAnswerPlacemark = country;
            break;
        }
        if ( linearring &&
            linearring->contains( coord ) &&
            linearring->contains(d->m_correctAnswer) )
        {
            foundClickedPoint = true;
            foundStandardPoint = true;
            d->m_correctAnswerPlacemark = country;
            break;
        }
        if ( multigeom ) {
            QVector<GeoDataGeometry*>::Iterator iter = multigeom->begin();
            QVector<GeoDataGeometry*>::Iterator const end = multigeom->end();

            for ( ; iter != end; ++iter ) {
                GeoDataPolygon *poly  = dynamic_cast<GeoDataPolygon*>( *iter );
                if ( poly &&
                    poly->contains( coord ) )
                {
                    foundClickedPoint = true;
                }
                if ( poly &&
                    poly->contains( d->m_correctAnswer ) )
                {
                    foundStandardPoint = true;
                    d->m_correctAnswerPlacemark = country;
                }
                if ( foundClickedPoint && foundStandardPoint ) {
                    break;
                }
            }
        }
        if ( foundClickedPoint && foundStandardPoint ) {
            break;
        }
    }

    if ( foundClickedPoint && foundStandardPoint ) {
        updateSelectPin( true, coord );
        emit updateResult( true );
    }
    else {
        updateSelectPin( false, coord );
        emit updateResult( false );
    }
}

void ClickOnThat::highlightCorrectAnswer()
{
    disconnect( d->m_marbleWidget, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)),
             this, SLOT(determineResult(qreal,qreal,GeoDataCoordinates::Unit)) );

    emit announceHighlight( d->m_correctAnswer.longitude(GeoDataCoordinates::Degree),
                            d->m_correctAnswer.latitude(GeoDataCoordinates::Degree),
                            GeoDataCoordinates::Degree );
    updateSelectPin( true, d->m_correctAnswer );

    /**
     * Zoom to highlighted placemark
     * so that it fits the current
     * view port
     */
    d->m_marbleWidget->centerOn( *(d->m_correctAnswerPlacemark), true );

    connect( d->m_marbleWidget, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)),
             this, SLOT(determineResult(qreal,qreal,GeoDataCoordinates::Unit)) );
}


}   // namespace Marble
