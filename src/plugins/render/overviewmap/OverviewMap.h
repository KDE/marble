// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>
//

#ifndef MARBLEOVERVIEWMAP_H
#define MARBLEOVERVIEWMAP_H

#include <QColor>
#include <QHash>
#include <QPixmap>
#include <QSvgRenderer>
#include <QSvgWidget>

#include "AbstractFloatItem.h"
#include "DialogConfigurationInterface.h"
#include "GeoDataLatLonAltBox.h"

namespace Ui
{
class OverviewMapConfigWidget;
}

namespace Marble
{

/**
 * @short The class that creates an overview map.
 *
 */

class OverviewMap : public AbstractFloatItem, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.OverviewMap")
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)
    MARBLE_PLUGIN(OverviewMap)

public:
    OverviewMap();
    explicit OverviewMap(const MarbleModel *marbleModel);
    ~OverviewMap() override;

    QStringList backendTypes() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    QDialog *configDialog() override;

    void initialize() override;

    bool isInitialized() const override;

    void setProjection(const ViewportParams *viewport) override;

    void paintContent(QPainter *painter) override;

    /**
     * @return: The settings of the item.
     */
    QHash<QString, QVariant> settings() const override;

    /**
     * Set the settings of the item.
     */
    void setSettings(const QHash<QString, QVariant> &settings) override;

public Q_SLOTS:
    void readSettings();
    void writeSettings();
    void updateSettings();

protected:
    bool eventFilter(QObject *object, QEvent *e) override;

private:
    void changeBackground(const QString &target);
    QSvgWidget *currentWidget() const;
    void setCurrentWidget(QSvgWidget *widget);
    void loadPlanetMaps();
    void loadMapSuggestions();

    QString m_target;
    QSvgRenderer m_svgobj;
    QHash<QString, QSvgWidget *> m_svgWidgets;
    QHash<QString, QString> m_svgPaths;
    QStringList m_planetID;
    QPixmap m_worldmap;
    QHash<QString, QVariant> m_settings;
    QColor m_posColor;
    QSizeF m_defaultSize;

    Ui::OverviewMapConfigWidget *ui_configWidget = nullptr;
    QDialog *m_configDialog;

    GeoDataLatLonAltBox m_latLonAltBox;
    qreal m_centerLat;
    qreal m_centerLon;
    bool m_mapChanged;

private Q_SLOTS:
    void chooseCustomMap();
    void synchronizeSpinboxes();
    void showCurrentPlanetPreview() const;
    void choosePositionIndicatorColor();
    void useMapSuggestion(int index);
};

}

#endif
