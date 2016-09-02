//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//


// Self
#include "GameMainWindow.h"
#include "CountryByShape.h"
#include "CountryByFlag.h"
#include "ClickOnThat.h"

#include "ui_game.h"

// Qt
#include <QSettings>
#include <QDebug>
#include <QString>
#include <QVector>
#include <QVBoxLayout>
#include <QQuickView>
#include <QQuickItem>
#include <QUrl>
#include <QSize>
#include <QResizeEvent>
#include <QFileInfo>

// Marble
#include <marble/MarbleDirs.h>
#include <marble/MarbleWidget.h>
#include <marble/MarbleMap.h>
#include <marble/MarbleModel.h>
#include <marble/GeoDataTreeModel.h>
#include <marble/RenderPlugin.h>
#include <marble/MarblePlacemarkModel.h>

#include <marble/GeoDataDocument.h>
#include <marble/GeoDataPlacemark.h>
#include <marble/GeoDataGeometry.h>
#include <marble/GeoDataMultiGeometry.h>
#include <marble/GeoDataPoint.h>
#include <marble/GeoDataCoordinates.h>
#include <marble/GeoDataPolygon.h>
#include <marble/GeoDataLinearRing.h>
#include <marble/GeoDataLatLonAltBox.h>
#include <marble/GeoDataTypes.h>

namespace Marble {

class Private : public Ui_MainWindow
{
public:
    Private( QWidget *parent = 0 );

    MarbleWidget *m_marbleWidget;
    QWidget *m_parent;
    QQuickView m_view;
    CountryByShape *m_countryByShape;
    CountryByFlag *m_countryByFlag;
    ClickOnThat *m_clickOnThat;

    QString readMarbleDataPath() const;
    void setupMarbleWidget();
    void setupGameSignals();
};

Private::Private(QWidget* parent) :
    m_marbleWidget( new MarbleWidget( parent ) ),
    m_parent( parent ),
    m_view(),
    m_countryByShape( new CountryByShape(m_marbleWidget) ),
    m_countryByFlag( new CountryByFlag(m_marbleWidget) ),
    m_clickOnThat( new ClickOnThat(m_marbleWidget) )
{
    // nothing to do
}

QString Private::readMarbleDataPath() const
{
    return QSettings().value(QStringLiteral("MarbleWidget/marbleDataPath"), QString()).toString();
}

void Private::setupMarbleWidget()
{
    m_marbleWidget->setMapThemeId(QStringLiteral( "earth/political/political.dgml"));

    foreach ( RenderPlugin *renderPlugin, m_marbleWidget->renderPlugins() ) {
        if (renderPlugin->nameId() == QLatin1String("stars")
            || renderPlugin->nameId() == QLatin1String("overviewmap")
            || renderPlugin->nameId() == QLatin1String("compass")
            || renderPlugin->nameId() == QLatin1String("scalebar")
            || renderPlugin->nameId() == QLatin1String("navigation"))
        {
            renderPlugin->setVisible( false );
        }
    }

    m_marbleWidget->centerOn( 23.0, 42.0 );
    m_marbleWidget->setDistance( 7500 );

    m_parent->connect( m_parent, SIGNAL(announceHighlight(qreal,qreal,GeoDataCoordinates::Unit)),
                       m_marbleWidget, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)) );
}

void Private::setupGameSignals()
{
    QObject *root = m_view.rootObject();
    if ( root ) {
        m_parent->connect( root, SIGNAL(browseMapButtonClicked()),
                           m_parent, SLOT(browseMapButtonClicked()) );
        QObject *gameOptions = root->findChild<QObject*>(QStringLiteral("gameOptions"));

        m_parent->connect( gameOptions, SIGNAL(nextButtonClicked()),
                           m_parent, SLOT(createQuestion()) );
        m_parent->connect( gameOptions, SIGNAL(gameClosed()),
                           m_parent, SLOT(disableGames()) );

        // For "Identify the highlighted country" game
        m_parent->connect( gameOptions, SIGNAL(countryByShapeGameRequested()),
                           m_parent, SLOT(enableCountryShapeGame()) );
        m_parent->connect( m_countryByShape, SIGNAL(gameInitialized()),
                           m_parent, SLOT(createQuestion()) );

        // For "Identify the flag" game
        m_parent->connect( gameOptions, SIGNAL(countryByFlagGameRequested()),
                           m_parent, SLOT(enableCountryFlagGame()) );
        m_parent->connect( m_countryByFlag, SIGNAL(gameInitialized()),
                           m_parent, SLOT(createQuestion()) );

        // For "Click on that country" game
        m_parent->connect( gameOptions, SIGNAL(clickOnThatGameRequested()),
                           m_parent, SLOT(enableClickOnThatGame()) );
        m_parent->connect( m_clickOnThat, SIGNAL(gameInitialized()),
                           m_parent, SLOT(createQuestion()) );
        m_parent->connect( gameOptions, SIGNAL(answerDisplayButtonClicked()),
                           m_clickOnThat, SLOT(highlightCorrectAnswer()) );
    }
}

MainWindow::MainWindow( const QString &marbleDataPath, QWidget *parent, Qt::WindowFlags flags ) :
    QMainWindow( parent, flags ),
    d( new Private( this ) )
{
    d->setupUi( this );
    QString const dataPath = marbleDataPath.isEmpty() ? d->readMarbleDataPath() : marbleDataPath;
    if ( !dataPath.isEmpty() ) {
        MarbleDirs::setMarbleDataPath( dataPath );
    }

    d->setupMarbleWidget();
    setCentralWidget( d->m_marbleWidget );

    d->m_view.setSource(QUrl(QStringLiteral("qrc:/Window.qml")));

    QWidget *leftPanel = QWidget::createWindowContainer( &d->m_view, d->dockWidgetContents );
    QVBoxLayout *layout = new QVBoxLayout( d->dockWidgetContents );
    layout->addWidget( leftPanel );
    d->dockWidgetContents->setLayout( layout );

    d->setupGameSignals();
}

MainWindow::~MainWindow()
{
    delete d;
}

MarbleWidget *MainWindow::marbleWidget()
{
    return d->m_marbleWidget;
}

void MainWindow::createQuestion()
{
    QObject *gameObject = d->m_view.rootObject()->findChild<QObject*>(QStringLiteral("gameOptions"));
    if ( gameObject ) {
        emit postQuestion( gameObject );
    }
}

void MainWindow::browseMapButtonClicked()
{
    d->m_marbleWidget->setMapThemeId(QStringLiteral("earth/political/political.dgml"));

    /**
     * Now display the country names which
     * were removed to initiate the game
     */
    const GeoDataTreeModel *const treeModel = d->m_marbleWidget->model()->treeModel();
    for ( int i = 0; i < treeModel->rowCount(); ++i ) {
        QVariant const data = treeModel->data ( treeModel->index ( i, 0 ), MarblePlacemarkModel::ObjectPointerRole );
        GeoDataObject *object = qvariant_cast<GeoDataObject*>( data );
        Q_ASSERT_X( object, "MainWindow::browseMapButtonClicked",
                    "failed to get valid data from treeModel for GeoDataObject" );
        if ( object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataDocument *doc = static_cast<GeoDataDocument*>( object );
            QFileInfo fileInfo( doc->fileName() );
            QString fileName = fileInfo.fileName();
            if (fileName == QLatin1String("boundaryplacemarks.cache")) {
                doc->setVisible( true );
                d->m_marbleWidget->model()->treeModel()->updateFeature( doc );
                d->m_marbleWidget->setHighlightEnabled( true );
                break;
            }
        }
    }
}

void MainWindow::disableGames()
{
    disconnect( this, SIGNAL(postQuestion(QObject*)),
                d->m_countryByShape, SLOT(postQuestion(QObject*)) );

    disconnect( this, SIGNAL(postQuestion(QObject*)),
                d->m_countryByFlag, SLOT(postQuestion(QObject*)) );

    disconnect( this, SIGNAL(postQuestion(QObject*)),
                d->m_clickOnThat, SLOT(postQuestion(QObject*)) );
    disconnect( d->m_clickOnThat, SIGNAL(updateResult(bool)),
                this, SLOT(displayResult(bool)) );
    disconnect( d->m_marbleWidget, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)),
                d->m_clickOnThat, SLOT(determineResult(qreal,qreal,GeoDataCoordinates::Unit)) );
    d->m_clickOnThat->disablePinDocument();

    // Reset the map view
    d->m_marbleWidget->centerOn( 23.0, 42.0 );
    d->m_marbleWidget->setDistance( 7500 );
}

void MainWindow::enableCountryShapeGame()
{
    connect( this, SIGNAL(postQuestion(QObject*)),
             d->m_countryByShape, SLOT(postQuestion(QObject*)) );

    d->m_countryByShape->initiateGame();
}

void MainWindow::enableCountryFlagGame()
{
    connect( this, SIGNAL(postQuestion(QObject*)),
             d->m_countryByFlag, SLOT(postQuestion(QObject*)) );

    d->m_countryByFlag->initiateGame();
}

void MainWindow::enableClickOnThatGame()
{
    connect( this, SIGNAL(postQuestion(QObject*)),
             d->m_clickOnThat, SLOT(postQuestion(QObject*)) );
    connect( d->m_clickOnThat, SIGNAL(updateResult(bool)),
             this, SLOT(displayResult(bool)) );
    d->m_clickOnThat->initiateGame();
}

void MainWindow::displayResult(bool result )
{
    QObject *gameObject = d->m_view.rootObject()->findChild<QObject*>(QStringLiteral("gameOptions"));
    if ( gameObject ) {
        QMetaObject::invokeMethod( gameObject, "displayResult",
                                   Q_ARG(QVariant, QVariant(result)) );
    }
}

/*
 * As the height of main window is changed, update the
 * height ( leftPanelHeight ) in window.qml
 */
void MainWindow::resizeEvent(QResizeEvent* event)
{
    const QSize size = event->size();

    QObject *root = d->m_view.rootObject();

    if ( root ) {
        QMetaObject::invokeMethod( root, "resizeWindow",
                                   Q_ARG(QVariant, QVariant(size.height()*9/10)) );
    }
}

}   // namespace Marble

#include "moc_GameMainWindow.cpp"
