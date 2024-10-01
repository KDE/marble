// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef PROGRESS_FLOAT_ITEM_H
#define PROGRESS_FLOAT_ITEM_H

#include "AbstractFloatItem.h"

#include <QIcon>
#include <QMutex>
#include <QTimer>

namespace Marble
{

/**
 * @brief A float item that shows a pie-chart progress
 * indicator when downloads are active
 */
class ProgressFloatItem : public AbstractFloatItem
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.ProgressFloatItem")

    Q_INTERFACES(Marble::RenderPluginInterface)

    MARBLE_PLUGIN(ProgressFloatItem)

public:
    explicit ProgressFloatItem(const MarbleModel *marbleModel = nullptr);
    ~ProgressFloatItem() override;

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

    void paintContent(QPainter *painter) override;

private Q_SLOTS:
    void removeProgressItem();

    void handleProgress(int active, int queued);

    void hideProgress();

    void show();

    void scheduleRepaint();

private:
    Q_DISABLE_COPY(ProgressFloatItem)

    bool active() const;

    void setActive(bool active);

    bool m_isInitialized;

    int m_totalJobs;

    int m_completedJobs;

    qreal m_completed;

    QTimer m_progressHideTimer;

    QTimer m_progressShowTimer;

    QMutex m_jobMutex;

    bool m_active;

    QIcon m_icon;

    int m_fontSize;

    QTimer m_repaintTimer;
};

}

#endif
