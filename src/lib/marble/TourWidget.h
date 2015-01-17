//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail.com>
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef MARBLE_TOURWIDGET_H
#define MARBLE_TOURWIDGET_H

#include "marble_export.h"

#include <QWidget>
#include <QStyledItemDelegate>
#include <QListView>
#include <GeoDataCoordinates.h>

class QItemSelection;
class QModelIndex;
class QDoubleSpinBox;
class QRadioButton;
class QLineEdit;
class QToolButton;
class QLabel;

namespace Marble
{

class FileManager;
class GeoDataPlacemark;
class GeoDataLatLonBox;
class GeoDataTreeModel;
class GeoDataFeature;
class GeoDataFlyTo;
class GeoDataTourControl;
class GeoDataWait;
class GeoDataSoundCue;
class GeoDataContainer;
class MarbleWidget;

class TourWidgetPrivate;

class MARBLE_EXPORT TourWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit TourWidget( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~TourWidget();

    void setMarbleWidget( MarbleWidget *widget );
    bool openTour( const QString &filename );

public Q_SLOTS:
    void startPlaying();
    void pausePlaying();
    void togglePlaying();
    void stopPlaying();
    void handleSliderMove( int );

Q_SIGNALS:
    void featureUpdated( GeoDataFeature *feature );
    void featureAdded( GeoDataContainer *parent, GeoDataFeature *feature, int row );
    void featureRemoved( const GeoDataFeature *feature  );

private Q_SLOTS:
    void moveUp();
    void moveDown();
    void addFlyTo();
    void addWait();
    void addSoundCue();
    void addPlacemark();
    void addRemovePlacemark();
    void addChangePlacemark();
    void deleteSelected();
    void updateDuration();
    void finishAddingItem();

 private:
    Q_PRIVATE_SLOT( d, void openFile() )
    Q_PRIVATE_SLOT( d, void createTour() )
    Q_PRIVATE_SLOT( d, void saveTour() )
    Q_PRIVATE_SLOT( d, void saveTourAs() )
    Q_PRIVATE_SLOT( d, void captureTour() )
    Q_PRIVATE_SLOT( d, void updateButtonsStates() )
    Q_PRIVATE_SLOT( d, void mapCenterOn( const QModelIndex &index ) )
    Q_PRIVATE_SLOT( d, void handlePlaybackProgress( const double position ) )
    Q_DISABLE_COPY( TourWidget )

    TourWidgetPrivate * const d;
};

}

#endif
