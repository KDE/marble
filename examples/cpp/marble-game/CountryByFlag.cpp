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
#include "CountryByFlag.h"

// Qt
#include <QTime>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QVector>
#include <QVariant>
#include <QVariantList>

// Marble
#include <MarbleWidget.h>
#include <MarbleModel.h>
#include <GeoDataTreeModel.h>
#include <MarbleDirs.h>
#include <MarbleDebug.h>
#include <MarblePlacemarkModel.h>

#include <GeoDataDocument.h>
#include <GeoDataPlacemark.h>

#include <GeoDataTypes.h>

namespace Marble
{
class CountryByFlagPrivate
{
public:
    CountryByFlagPrivate( MarbleWidget *marbleWidget )
    : m_marbleWidget( marbleWidget ),
      m_parent( 0 ),
      m_countryNames( 0 )
    {
        m_continentsAndOceans << "Asia" << "Africa" << "North America" << "South America"
        << "Antarctica" << "Europe" << "Australia" << "Arctic Ocean" << "Indian Ocean"
        << "North Atlantic Ocean" << "North Pacific Ocean" << "South Pacific Ocean"
        << "South Atlantic Ocean" << "Southern Ocean" ;
    }

    MarbleWidget *m_marbleWidget;
    CountryByFlag *m_parent;

    /**
     * Document to store point placemarks which
     * have country names ( from file "boundaryplacemarks.cache" )
     */
    GeoDataDocument *m_countryNames;

    /*
     * When I select a random placemark form boundaryplacemarks.cache
     * it may represent a continent. Since there is no flag
     * for a continent, we will not use this placemark to post question.
     * This list will help checking whether the placemark choosen to
     * post question is a continent/ocean .
     */
    QStringList m_continentsAndOceans;
};

CountryByFlag::CountryByFlag( MarbleWidget *marbleWidget )
    : QObject(),
      d ( new CountryByFlagPrivate(marbleWidget) )
{
    d->m_parent = this;
}

CountryByFlag::~CountryByFlag()
{
    delete d->m_countryNames;
    delete d;
}

void CountryByFlag::initiateGame()
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
            Q_ASSERT_X( object, "CountryByFlag::initiateGame",
                        "failed to get valid data from treeModel for GeoDataObject" );
            if ( object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
                GeoDataDocument *doc = static_cast<GeoDataDocument*>( object );
                QFileInfo fileInfo( doc->fileName() );
                if ( fileInfo.fileName() == QString("boundaryplacemarks.cache") ) {
                    d->m_countryNames = doc;
                    break;
                }
            }
        }
    }

    if ( d->m_countryNames ) {
        d->m_countryNames->setVisible( false );
        d->m_marbleWidget->model()->treeModel()->updateFeature( d->m_countryNames );
        d->m_marbleWidget->centerOn( 23.0, 42.0 );
        d->m_marbleWidget->setDistance( 7500 );
        d->m_marbleWidget->setHighlightEnabled( false );
        emit gameInitialized();
    }
}

void CountryByFlag::postQuestion( QObject *gameObject )
{
    /**
     * Find a random placemark
     */
    Q_ASSERT_X( d->m_countryNames, "CountryByFlag::postQuestion",
                "CountryByFlagPrivate::m_countryNames is NULL" );
    QVector<GeoDataPlacemark*> countryPlacemarks = d->m_countryNames->placemarkList();

    uint randomSeed = uint(QTime::currentTime().msec());
    qsrand( randomSeed );

    bool found = false;
    GeoDataPlacemark *placemark = 0;
    QVariantList answerOptions;
    QString flagPath;

    while ( !found ) {
        int randomIndex = qrand()%countryPlacemarks.size();
        placemark = countryPlacemarks[randomIndex];

        if ( !d->m_continentsAndOceans.contains(placemark->name(), Qt::CaseSensitive) ) {
            flagPath = MarbleDirs::path( QString("flags/flag_%1.svg").arg(placemark->countryCode().toLower()) );
            QImage flag = QFile::exists( flagPath ) ? QImage( flagPath ) : QImage();
            if ( !flag.isNull() ) {
                flagPath = QString("%1flag_%2.svg").arg("../../../data/flags/").arg(placemark->countryCode().toLower());
                found = true;
            }
        }
    }

    answerOptions << placemark->name()
    << countryPlacemarks[qrand()%countryPlacemarks.size()]->name()
    << countryPlacemarks[qrand()%countryPlacemarks.size()]->name()
    << countryPlacemarks[qrand()%countryPlacemarks.size()]->name();

    // Randomize the options in the list answerOptions
    for ( int i = 0; i < answerOptions.size(); ++i ) {
        QVariant option = answerOptions.takeAt( qrand()%answerOptions.size() );
        answerOptions.append( option );
    }
    if ( gameObject ) {
        QMetaObject::invokeMethod( gameObject, "countryByFlagQuestion",
                                   Q_ARG(QVariant, QVariant::fromValue(answerOptions)),
                                   Q_ARG(QVariant, QVariant::fromValue(flagPath)),
                                   Q_ARG(QVariant, QVariant::fromValue(placemark->name())) );
    }
}

}   // namespace Marble

#include "CountryByFlag.moc"
