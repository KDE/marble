// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2011 Michael Henning <mikehenning@eclipse.net>
//

//
// MeasureToolPlugin enables Marble to set and display measure points
//

#ifndef MARBLE_MEASURETOOLPLUGIN_H
#define MARBLE_MEASURETOOLPLUGIN_H

#include "DialogConfigurationInterface.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLineString.h"
#include "MarbleWidget.h"
#include "RenderPlugin.h"

#include <QAction>
#include <QFont>
#include <QPen>
#include <QPixmap>

namespace Marble
{

class MeasureConfigDialog;
class MeasureToolPlugin : public RenderPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.MeasureToolPlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)
    MARBLE_PLUGIN(MeasureToolPlugin)

public:
    explicit MeasureToolPlugin(const MarbleModel *marbleModel = nullptr);

    enum PaintMode { Polygon = 0, Circular };

    QStringList backendTypes() const override;
    QString renderPolicy() const override;
    QStringList renderPosition() const override;
    QString name() const override;
    QString guiString() const override;
    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    void initialize() override;

    bool isInitialized() const override;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer = nullptr) override;

    QDialog *configDialog() override;
    QHash<QString, QVariant> settings() const override;
    void setSettings(const QHash<QString, QVariant> &settings) override;

Q_SIGNALS:
    void numberOfMeasurePointsChanged(int newNumber);

public Q_SLOTS:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    void drawMeasurePoints(GeoPainter *painter);
    void drawInfobox(GeoPainter *painter) const;
    void drawSegments(GeoPainter *painter);
    void addContextItems();
    void removeContextItems();

private Q_SLOTS:
    void setNumberOfMeasurePoints(int number);
    void addMeasurePointEvent();

    void addMeasurePoint(qreal lon, qreal lat);
    void removeLastMeasurePoint();
    void removeMeasurePoints();

    void writeSettings();

private:
    Q_DISABLE_COPY(MeasureToolPlugin)

    static QString meterToPreferredUnit(qreal meters, bool isSquare = false);

    // The line strings in the distance path.
    GeoDataLineString m_measureLineString;
    GeoDataLatLonAltBox m_latLonAltBox;

    QPixmap m_mark;
    QFont m_font_regular;
    int m_fontascent;

    QPen m_pen;

    QAction *m_addMeasurePointAction = nullptr;
    QAction *m_removeLastMeasurePointAction = nullptr;
    QAction *m_removeMeasurePointsAction = nullptr;
    QAction *m_separator = nullptr;

    MarbleWidget *m_marbleWidget = nullptr;

    MeasureConfigDialog *m_configDialog = nullptr;

    bool m_showDistanceLabel;
    bool m_showBearingLabel;
    bool m_showBearingChangeLabel;

    bool m_showPolygonArea;
    bool m_showCircularArea;
    bool m_showRadius;
    bool m_showPerimeter;
    bool m_showCircumference;

    qreal m_totalDistance;
    qreal m_polygonArea;
    qreal m_circularArea;
    qreal m_radius;
    qreal m_perimeter;
    qreal m_circumference;

    PaintMode m_paintMode;
};

}

#endif // MARBLE_MEASURETOOLPLUGIN_H
