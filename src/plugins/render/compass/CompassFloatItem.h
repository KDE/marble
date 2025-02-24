// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>
//

#ifndef COMPASS_FLOAT_ITEM_H
#define COMPASS_FLOAT_ITEM_H

#include <QPixmap>

#include "AbstractFloatItem.h"
#include "DialogConfigurationInterface.h"

class QSvgRenderer;

namespace Ui
{
class CompassConfigWidget;
}

namespace Marble
{

/**
 * @short The class that creates a compass
 *
 */

class CompassFloatItem : public AbstractFloatItem, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.CompassFloatItem")
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)
    MARBLE_PLUGIN(CompassFloatItem)
public:
    CompassFloatItem();
    explicit CompassFloatItem(const MarbleModel *marbleModel);
    ~CompassFloatItem() override;

    QStringList backendTypes() const override;

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

    QPainterPath backgroundShape() const override;

    void setProjection(const ViewportParams *viewport) override;

    void paintContent(QPainter *painter) override;

    QDialog *configDialog() override;

    QHash<QString, QVariant> settings() const override;

    void setSettings(const QHash<QString, QVariant> &settings) override;

private Q_SLOTS:
    void readSettings();

    void writeSettings();

private:
    Q_DISABLE_COPY(CompassFloatItem)

    bool m_isInitialized;

    QSvgRenderer *m_svgobj = nullptr;
    QPixmap m_compass;

    /// allowed values: -1, 0, 1; default here: 0. FIXME: Declare enum
    int m_polarity;

    int m_themeIndex;
    QDialog *m_configDialog = nullptr;
    Ui::CompassConfigWidget *m_uiConfigWidget = nullptr;
};
}

#endif
