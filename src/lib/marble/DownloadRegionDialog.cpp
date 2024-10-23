// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//

#include "DownloadRegionDialog.h"

#include <cmath>

#include <QButtonGroup>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSet>
#include <QShowEvent>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QTimer>
#include <QVBoxLayout>

#include "DownloadRegion.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLineString.h"
#include "GeoSceneDocument.h"
#include "GeoSceneMap.h"
#include "LatLonBoxWidget.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "Route.h"
#include "TextureLayer.h"
#include "TileCoordsPyramid.h"
#include "TileId.h"
#include "TileLayer.h"
#include "TileLevelRangeWidget.h"
#include "TileLoaderHelper.h"
#include "VectorTileLayer.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"

namespace Marble
{

int const maxTilesCount = 100000;
int const minimumRouteOffset = 0;
int const maximumRouteOffset = 10000;
int averageTextureTileSize = 13; // The average size of a tile in kilobytes
int averageVectorTileSize = 30; // The average size of a vector tile in kilobytes

class Q_DECL_HIDDEN DownloadRegionDialog::Private
{
public:
    Private(MarbleWidget *const widget, QDialog *const dialog);
    QWidget *createSelectionMethodBox();
    QLayout *createTilesCounter();
    QWidget *createOkCancelButtonBox();

    bool hasRoute() const;
    bool hasTextureLayers() const;
    bool hasVectorLayers() const;
    QDialog *m_dialog;
    QLabel *m_layerLabel;
    QComboBox *m_layerComboBox;
    QButtonGroup *m_buttonGroup;
    QRadioButton *m_visibleRegionMethodButton;
    QRadioButton *m_specifiedRegionMethodButton;
    LatLonBoxWidget *m_latLonBoxWidget;
    TileLevelRangeWidget *m_tileLevelRangeWidget;
    QRadioButton *m_routeDownloadMethodButton;
    QLabel *m_routeOffsetLabel;
    QDoubleSpinBox *m_routeOffsetSpinBox;
    QLabel *m_tilesCountLabel;
    QLabel *m_tileSizeInfo;
    QPushButton *m_okButton;
    QPushButton *m_applyButton;
    TextureLayer const *m_textureLayer;
    VectorTileLayer const *m_vectorTileLayer;
    int m_visibleTileLevel;
    MarbleModel const *const m_model;
    MarbleWidget *const m_widget;
    SelectionMethod m_selectionMethod;
    GeoDataLatLonAltBox m_visibleRegion;
    RoutingModel *m_routingModel;
    DownloadRegion m_downloadRegion;
    TileType m_tileType;
};

DownloadRegionDialog::Private::Private(MarbleWidget *const widget, QDialog *const dialog)
    : m_dialog(dialog)
    , m_layerLabel(nullptr)
    , m_layerComboBox(nullptr)
    , m_buttonGroup(nullptr)
    , m_visibleRegionMethodButton(nullptr)
    , m_specifiedRegionMethodButton(nullptr)
    , m_latLonBoxWidget(new LatLonBoxWidget)
    , m_tileLevelRangeWidget(new TileLevelRangeWidget)
    , m_routeDownloadMethodButton(nullptr)
    , m_routeOffsetLabel(nullptr)
    , m_routeOffsetSpinBox(nullptr)
    , m_tilesCountLabel(nullptr)
    , m_tileSizeInfo(nullptr)
    , m_okButton(nullptr)
    , m_applyButton(nullptr)
    , m_textureLayer(widget->textureLayer())
    , m_vectorTileLayer(widget->vectorTileLayer())
    , m_visibleTileLevel(0)
    , m_model(widget->model())
    , m_widget(widget)
    , m_selectionMethod(VisibleRegionMethod)
    , m_visibleRegion()
    , m_routingModel(widget->model()->routingManager()->routingModel())
{
    m_latLonBoxWidget->setEnabled(false);
    m_latLonBoxWidget->setLatLonBox(m_visibleRegion);
    m_tileLevelRangeWidget->setDefaultLevel(m_visibleTileLevel);
    m_downloadRegion.setMarbleModel(widget->model());
}

QWidget *DownloadRegionDialog::Private::createSelectionMethodBox()
{
    m_buttonGroup = new QButtonGroup(m_dialog);
    m_buttonGroup->setExclusive(true);
    m_visibleRegionMethodButton = new QRadioButton(tr("Visible region"));
    m_buttonGroup->addButton(m_visibleRegionMethodButton);
    m_specifiedRegionMethodButton = new QRadioButton(tr("Specify region"));
    m_buttonGroup->addButton(m_specifiedRegionMethodButton);
    m_routeDownloadMethodButton = new QRadioButton(tr("Download Route"));
    m_buttonGroup->addButton(m_routeDownloadMethodButton);
    m_routeDownloadMethodButton->setToolTip(tr("Enabled when a route exists"));
    m_routeDownloadMethodButton->setEnabled(hasRoute());
    m_routeDownloadMethodButton->setChecked(hasRoute());
    m_routeOffsetSpinBox = new QDoubleSpinBox();
    m_routeOffsetSpinBox->setEnabled(hasRoute());
    m_routeOffsetSpinBox->setRange(minimumRouteOffset, maximumRouteOffset);
    int defaultOffset = 500;
    m_routeOffsetSpinBox->setValue(defaultOffset);
    m_routeOffsetSpinBox->setSingleStep(100);
    m_routeOffsetSpinBox->setSuffix(QStringLiteral(" m"));
    m_routeOffsetSpinBox->setDecimals(0);
    m_routeOffsetSpinBox->setAlignment(Qt::AlignRight);

    m_routeOffsetLabel = new QLabel(tr("Offset from route:"));
    m_routeOffsetLabel->setAlignment(Qt::AlignHCenter);

    connect(m_buttonGroup, SIGNAL(buttonToggled(QAbstractButton *, bool)), m_dialog, SLOT(toggleSelectionMethod()));
    connect(m_routingModel, SIGNAL(modelReset()), m_dialog, SLOT(updateRouteDialog()));
    connect(m_routingModel, SIGNAL(rowsInserted(QModelIndex, int, int)), m_dialog, SLOT(updateRouteDialog()));
    connect(m_routingModel, SIGNAL(rowsRemoved(QModelIndex, int, int)), m_dialog, SLOT(updateRouteDialog()));

    auto routeOffsetLayout = new QHBoxLayout;
    routeOffsetLayout->addWidget(m_routeOffsetLabel);
    routeOffsetLayout->insertSpacing(0, 25);
    routeOffsetLayout->addWidget(m_routeOffsetSpinBox);

    auto const routeLayout = new QVBoxLayout;
    routeLayout->addWidget(m_routeDownloadMethodButton);
    routeLayout->addLayout(routeOffsetLayout);

    auto const layout = new QVBoxLayout;
    layout->addWidget(m_visibleRegionMethodButton);
    layout->addLayout(routeLayout);
    layout->addWidget(m_specifiedRegionMethodButton);
    layout->addWidget(m_latLonBoxWidget);

    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    m_specifiedRegionMethodButton->setVisible(!smallScreen);
    m_latLonBoxWidget->setVisible(!smallScreen);

    if (smallScreen) {
        auto const selectionMethodWidget = new QWidget;
        selectionMethodWidget->setLayout(layout);
        return selectionMethodWidget;
    } else {
        auto const selectionMethodBox = new QGroupBox(tr("Selection Method"));
        selectionMethodBox->setLayout(layout);
        return selectionMethodBox;
    }
}

QLayout *DownloadRegionDialog::Private::createTilesCounter()
{
    auto const description = new QLabel(tr("Number of tiles to download:"));
    m_tilesCountLabel = new QLabel;
    m_tileSizeInfo = new QLabel;

    auto const tilesCountLayout = new QHBoxLayout;
    tilesCountLayout->addWidget(description);
    tilesCountLayout->addWidget(m_tilesCountLabel);
    // tilesCountLayout->insertSpacing( 0, 5 );
    auto const layout = new QVBoxLayout;
    layout->addLayout(tilesCountLayout);
    layout->addWidget(m_tileSizeInfo);
    return layout;
}

QWidget *DownloadRegionDialog::Private::createOkCancelButtonBox()
{
    auto const buttonBox = new QDialogButtonBox;
    m_okButton = buttonBox->addButton(QDialogButtonBox::Ok);
    m_applyButton = buttonBox->addButton(QDialogButtonBox::Apply);
    if (MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen) {
        buttonBox->removeButton(m_applyButton);
        m_applyButton->setVisible(false);
    }
    buttonBox->addButton(QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), m_dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), m_dialog, SLOT(reject()));
    connect(m_applyButton, SIGNAL(clicked()), m_dialog, SIGNAL(applied()));
    return buttonBox;
}

bool DownloadRegionDialog::Private::hasRoute() const
{
    return !m_routingModel->route().path().isEmpty();
}

bool DownloadRegionDialog::Private::hasTextureLayers() const
{
    return m_model->mapTheme()->map()->hasTextureLayers();
}

bool DownloadRegionDialog::Private::hasVectorLayers() const
{
    return m_model->mapTheme()->map()->hasVectorLayers();
}

DownloadRegionDialog::DownloadRegionDialog(MarbleWidget *const widget, QWidget *const parent, Qt::WindowFlags const f)
    : QDialog(parent, f)
    , d(new Private(widget, this))
{
    setWindowTitle(tr("Download Region"));
    auto const layout = new QVBoxLayout;
    d->m_layerLabel = new QLabel(tr("Tile type to be downloaded:"));
    d->m_layerComboBox = new QComboBox();
    d->m_layerComboBox->addItem(tr("Texture tiles"));
    d->m_layerComboBox->addItem(tr("Vector tiles"));
    d->m_layerComboBox->setToolTip(tr("Allows selection between layer types that are visibly being rendered."));
    updateTileLayer();

    layout->addWidget(d->m_layerLabel);
    layout->addWidget(d->m_layerComboBox);
    layout->addWidget(d->createSelectionMethodBox());
    layout->addWidget(d->m_tileLevelRangeWidget);
    layout->addStretch();
    layout->addLayout(d->createTilesCounter());

    if (MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen) {
        auto widget = new QWidget(this);
        widget->setLayout(layout);
        auto scrollArea = new QScrollArea(this);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidget(widget);
        auto const mainLayout = new QVBoxLayout;
        mainLayout->addWidget(scrollArea);
        mainLayout->addWidget(d->createOkCancelButtonBox());
        setLayout(mainLayout);
    } else {
        layout->addWidget(d->createOkCancelButtonBox());
        setLayout(layout);
    }

    connect(d->m_layerComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DownloadRegionDialog::updateTileCount);
    connect(d->m_latLonBoxWidget, &Marble::LatLonBoxWidget::valueChanged, this, &DownloadRegionDialog::updateTileCount);
    connect(d->m_tileLevelRangeWidget, &TileLevelRangeWidget::topLevelChanged, this, &DownloadRegionDialog::updateTileCount);
    connect(d->m_tileLevelRangeWidget, &TileLevelRangeWidget::bottomLevelChanged, this, &DownloadRegionDialog::updateTileCount);
    connect(d->m_routeOffsetSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &DownloadRegionDialog::updateTileCount);
    connect(d->m_routeOffsetSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &DownloadRegionDialog::setOffsetUnit);
    connect(d->m_model, &MarbleModel::themeChanged, this, &DownloadRegionDialog::delayUpdateTileLayer);
}

DownloadRegionDialog::~DownloadRegionDialog()
{
    delete d;
}

void DownloadRegionDialog::setAllowedTileLevelRange(int const minimumTileLevel, int const maximumTileLevel)
{
    d->m_tileLevelRangeWidget->setAllowedLevelRange(minimumTileLevel, maximumTileLevel);
}

void DownloadRegionDialog::setVisibleTileLevel(int const tileLevel)
{
    d->m_visibleTileLevel = tileLevel;
    d->m_tileLevelRangeWidget->setDefaultLevel(tileLevel);
    d->m_downloadRegion.setVisibleTileLevel(tileLevel);
}

void DownloadRegionDialog::setSelectionMethod(SelectionMethod const selectionMethod)
{
    d->m_selectionMethod = selectionMethod;
    switch (selectionMethod) {
    case VisibleRegionMethod:
        d->m_visibleRegionMethodButton->setChecked(true);
        d->m_routeOffsetLabel->setEnabled(false);
        d->m_routeOffsetSpinBox->setEnabled(false);
        d->m_latLonBoxWidget->setEnabled(false);
        setSpecifiedLatLonAltBox(d->m_visibleRegion);
        break;
    case SpecifiedRegionMethod:
        d->m_specifiedRegionMethodButton->setChecked(true);
        d->m_routeOffsetLabel->setEnabled(false);
        d->m_routeOffsetSpinBox->setEnabled(false);
        d->m_latLonBoxWidget->setEnabled(true);
        break;
    case RouteDownloadMethod:
        d->m_routeDownloadMethodButton->setChecked(true);
        d->m_routeOffsetLabel->setEnabled(true);
        d->m_routeOffsetSpinBox->setEnabled(true);
        d->m_latLonBoxWidget->setEnabled(false);
    }

    updateTileCount();
}

QList<TileCoordsPyramid> DownloadRegionDialog::region() const
{
    if (!d->hasTextureLayers() && !d->hasVectorLayers()) {
        return {};
    }

    d->m_visibleTileLevel = (tileType() == TextureTileType && d->m_textureLayer->tileZoomLevel() != -1) ? d->m_textureLayer->tileZoomLevel()
                                                                                                        : d->m_vectorTileLayer->tileZoomLevel();

    const TileLayer *tileLayer = (tileType() == TextureTileType && d->m_textureLayer->layerCount() > 0) ? dynamic_cast<const TileLayer *>(d->m_textureLayer)
                                                                                                        : dynamic_cast<const TileLayer *>(d->m_vectorTileLayer);

    d->m_downloadRegion.setTileLevelRange(d->m_tileLevelRangeWidget->topLevel(), d->m_tileLevelRangeWidget->bottomLevel());
    d->m_downloadRegion.setVisibleTileLevel(d->m_visibleTileLevel);

    // check whether "visible region" or "lat/lon region" is selection method
    GeoDataLatLonAltBox downloadRegion;
    switch (d->m_selectionMethod) {
    case VisibleRegionMethod:
        downloadRegion = d->m_visibleRegion;
        break;
    case SpecifiedRegionMethod:
        downloadRegion = GeoDataLatLonAltBox(d->m_latLonBoxWidget->latLonBox(), 0, 0);
        break;
    case RouteDownloadMethod:
        qreal offset = d->m_routeOffsetSpinBox->value();
        if (d->m_routeOffsetSpinBox->suffix() == QLatin1StringView(" km")) {
            offset *= KM2METER;
        }
        const GeoDataLineString waypoints = d->m_model->routingManager()->routingModel()->route().path();
        return d->m_downloadRegion.fromPath(tileLayer, offset, waypoints);
    }

    // For Mercator tiles limit the LatLonBox to the valid tile range.
    if (tileLayer->tileProjection()->type() == GeoSceneAbstractTileProjection::Mercator) {
        downloadRegion.setNorth(qMin(downloadRegion.north(), +1.4835));
        downloadRegion.setSouth(qMax(downloadRegion.south(), -1.4835));
    }

    return d->m_downloadRegion.region(tileLayer, downloadRegion);
}

TileType DownloadRegionDialog::tileType() const
{
    return d->m_layerComboBox->currentIndex() == 0 ? TextureTileType : VectorTileType;
}

void DownloadRegionDialog::setSpecifiedLatLonAltBox(GeoDataLatLonAltBox const &region)
{
    d->m_latLonBoxWidget->setLatLonBox(region);
}

void DownloadRegionDialog::setVisibleLatLonAltBox(GeoDataLatLonAltBox const &region)
{
    d->m_visibleRegion = region;

    // update lat/lon widget only if not active to prevent that users unintentionally loose
    // entered values
    if (d->m_selectionMethod == VisibleRegionMethod) {
        setSpecifiedLatLonAltBox(region);
    }
    updateTileCount();
}

void DownloadRegionDialog::updateTileLayer()
{
    updateTileType();
    updateTileCount();
}

void DownloadRegionDialog::delayUpdateTileLayer()
{
    QTimer::singleShot(500, this, &DownloadRegionDialog::updateTileLayer);
}

void DownloadRegionDialog::hideEvent(QHideEvent *event)
{
    disconnect(d->m_widget, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)), this, SLOT(setVisibleLatLonAltBox(GeoDataLatLonAltBox)));
    disconnect(d->m_widget, SIGNAL(themeChanged(QString)), this, SLOT(delayUpdateTileLayer()));
    disconnect(d->m_widget, SIGNAL(propertyValueChanged(QString, bool)), this, SLOT(delayUpdateTileLayer()));

    Q_EMIT hidden();
    event->accept();
}

void DownloadRegionDialog::showEvent(QShowEvent *event)
{
    connect(d->m_widget, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)), this, SLOT(setVisibleLatLonAltBox(GeoDataLatLonAltBox)));
    connect(d->m_widget, SIGNAL(themeChanged(QString)), this, SLOT(delayUpdateTileLayer()));
    connect(d->m_widget, SIGNAL(propertyValueChanged(QString, bool)), this, SLOT(delayUpdateTileLayer()));

    setVisibleTileLevel(d->m_widget->tileZoomLevel());

    updateTileCount();

    Q_EMIT shown();
    event->accept();
}

void DownloadRegionDialog::toggleSelectionMethod()
{
    if (d->m_specifiedRegionMethodButton->isChecked()) {
        setSelectionMethod(SpecifiedRegionMethod);
    } else if (d->m_routeDownloadMethodButton->isChecked()) {
        setSelectionMethod(RouteDownloadMethod);
    } else if (d->m_specifiedRegionMethodButton->isChecked()) {
        setSelectionMethod(SpecifiedRegionMethod);
    }
}

void DownloadRegionDialog::updateTileType()
{
    bool hasVisibleTextureLayers = d->hasTextureLayers() && d->m_textureLayer->layerCount() > 0;
    bool hasVisibleVectorLayers = d->hasVectorLayers() && d->m_vectorTileLayer->layerCount() > 0;

    auto model = qobject_cast<QStandardItemModel *>(d->m_layerComboBox->model());
    Q_ASSERT(model != nullptr);
    QStandardItem *item = nullptr;
    item = model->item(0);
    item->setFlags(hasVisibleTextureLayers ? item->flags() | Qt::ItemIsEnabled : item->flags() & ~Qt::ItemIsEnabled);
    item = model->item(1);
    item->setFlags(hasVisibleVectorLayers ? item->flags() | Qt::ItemIsEnabled : item->flags() & ~Qt::ItemIsEnabled);

    bool allTileTypesAvailable = hasVisibleTextureLayers && hasVisibleVectorLayers;

    d->m_layerComboBox->setEnabled(allTileTypesAvailable);
    if (hasVisibleVectorLayers) {
        d->m_layerComboBox->setCurrentIndex(1);
    } else if (hasVisibleTextureLayers && !hasVisibleVectorLayers) {
        d->m_layerComboBox->setCurrentIndex(0);
    }
}

void DownloadRegionDialog::updateTileCount()
{
    if (!isVisible()) {
        return;
    }

    qint64 tilesCount = 0;
    QString themeId(d->m_model->mapThemeId());
    QList<TileCoordsPyramid> const pyramid = region();
    Q_ASSERT(!pyramid.isEmpty());
    if (pyramid.size() == 1) {
        tilesCount = pyramid[0].tilesCount();
    } else {
        for (int level = pyramid[0].bottomLevel(); level >= pyramid[0].topLevel(); --level) {
            QSet<TileId> tileIdSet;
            for (int i = 0; i < pyramid.size(); ++i) {
                QRect const coords = pyramid[i].coords(level);
                int x1, y1, x2, y2;
                coords.getCoords(&x1, &y1, &x2, &y2);
                for (int x = x1; x <= x2; ++x) {
                    for (int y = y1; y <= y2; ++y) {
                        TileId const tileId(0, level, x, y);
                        tileIdSet.insert(tileId);
                    }
                }
            }
            tilesCount += tileIdSet.count();
        }
    }

    qreal tileDownloadSize = 0;

    if (tilesCount > maxTilesCount) {
        d->m_tileSizeInfo->setToolTip(QString());
        //~ singular There is a limit of %n tile to download.
        //~ plural There is a limit of %n tiles to download.
        d->m_tileSizeInfo->setText(tr("There is a limit of %n tile(s) to download.", "", maxTilesCount));
    } else {
        if (themeId == QLatin1StringView("earth/openstreetmap/openstreetmap.dgml") || themeId == QLatin1StringView("earth/openstreetmap/openseamap.dgml")
            || themeId == QLatin1StringView("earth/vectorosm/vectorosm.dgml")) {
            tileDownloadSize = tileType() == TextureTileType ? tilesCount * averageTextureTileSize : tilesCount * averageVectorTileSize;

            d->m_tileSizeInfo->setToolTip(tr("Approximate size of the tiles to be downloaded"));

            if (tileDownloadSize > 1024) {
                tileDownloadSize = tileDownloadSize / 1024;
                d->m_tileSizeInfo->setText(tr("Estimated download size: %1 MB").arg(ceil(tileDownloadSize)));
            } else {
                d->m_tileSizeInfo->setText(tr("Estimated download size: %1 kB").arg(tileDownloadSize));
            }
        } else {
            d->m_tileSizeInfo->setToolTip(QString());
            d->m_tileSizeInfo->clear();
        }
    }

    d->m_tilesCountLabel->setText(QString::number(tilesCount));
    bool const tilesCountWithinLimits = tilesCount > 0 && tilesCount <= maxTilesCount;
    d->m_okButton->setEnabled(tilesCountWithinLimits);
    d->m_applyButton->setEnabled(tilesCountWithinLimits);
}

void DownloadRegionDialog::updateRouteDialog()
{
    d->m_routeDownloadMethodButton->setEnabled(d->hasRoute());
    d->m_routeDownloadMethodButton->setChecked(d->hasRoute());
    if (!d->hasRoute()) {
        setSelectionMethod(VisibleRegionMethod);
    }
}

void DownloadRegionDialog::setOffsetUnit()
{
    qreal offset = d->m_routeOffsetSpinBox->value();

    if (offset >= 1100) {
        d->m_routeOffsetSpinBox->setSuffix(QStringLiteral(" km"));
        d->m_routeOffsetSpinBox->setRange(minimumRouteOffset * METER2KM, maximumRouteOffset * METER2KM);
        d->m_routeOffsetSpinBox->setDecimals(1);
        d->m_routeOffsetSpinBox->setValue(offset * METER2KM);
        d->m_routeOffsetSpinBox->setSingleStep(0.1);
    } else if (offset <= 1 && d->m_routeOffsetSpinBox->suffix() == QLatin1StringView(" km")) {
        d->m_routeOffsetSpinBox->setSuffix(QStringLiteral(" m"));
        d->m_routeOffsetSpinBox->setRange(minimumRouteOffset, maximumRouteOffset);
        d->m_routeOffsetSpinBox->setDecimals(0);
        d->m_routeOffsetSpinBox->setValue(offset * KM2METER);
        d->m_routeOffsetSpinBox->setSingleStep(100);
    }
}

}

#include "moc_DownloadRegionDialog.cpp"
