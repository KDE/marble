// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Andrew Manson <g.real.ate@gmail.com>
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
// SPDX-FileCopyrightText: 2010 Thibaut Gridel <tgridel@free.fr>
//

#ifndef POSITION_MARKER_H
#define POSITION_MARKER_H

#include <QColor>
#include <QHash>
#include <QList>
#include <QPixmap>
#include <QPolygon>

#include "DialogConfigurationInterface.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "RenderPlugin.h"

namespace Ui
{
class PositionMarkerConfigWidget;
}

namespace Marble
{

class PositionMarker : public RenderPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.PositionMarker")
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)
    MARBLE_PLUGIN(PositionMarker)
public:
    explicit PositionMarker(const MarbleModel *marbleModel = nullptr);
    ~PositionMarker() override;

    QStringList renderPosition() const override;

    QString renderPolicy() const override;

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

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer = nullptr) override;

    // Overriding LayerInterface to paint on top of the route
    qreal zValue() const override;

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

    void setPosition(const GeoDataCoordinates &position);
    void chooseCustomCursor();
    void chooseColor();
    void resizeCursor(int step);

private:
    Q_DISABLE_COPY(PositionMarker)

    void loadCustomCursor(const QString &filename, bool useCursor);
    void loadDefaultCursor();

    const MarbleModel *m_marbleModel;

    bool m_isInitialized;
    bool m_useCustomCursor;

    const QString m_defaultCursorPath;
    GeoDataLatLonAltBox m_lastBoundingBox;
    GeoDataCoordinates m_currentPosition;
    GeoDataCoordinates m_previousPosition;

    Ui::PositionMarkerConfigWidget *ui_configWidget;
    QDialog *m_configDialog;
    QString m_cursorPath;

    QPolygonF m_arrow;
    QPolygonF m_previousArrow;
    QRegion m_dirtyRegion;
    QPixmap m_customCursor;
    QPixmap m_customCursorTransformed;
    QPixmap m_defaultCursor;
    float m_cursorSize;
    QColor m_accuracyColor;
    QColor m_trailColor;
    qreal m_heading;
    QList<GeoDataCoordinates> m_trail;
    static const int sm_numTrailPoints = 6;
    bool m_showTrail;

    static const int sm_defaultSizeStep;
    static const int sm_numResizeSteps;
    static const float sm_resizeSteps[];
};

}

#endif
