// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_SEARCHWIDGET_H
#define MARBLE_SEARCHWIDGET_H

#include "marble_export.h"

#include <QWidget>

namespace Marble
{

class GeoDataPlacemark;
class MarbleWidget;
class SearchWidgetPrivate;

class MARBLE_EXPORT SearchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    void setMarbleWidget(MarbleWidget *widget);

    ~SearchWidget() override;

private:
    Q_PRIVATE_SLOT(d, void setSearchResult(const QList<GeoDataPlacemark *> &))
    Q_PRIVATE_SLOT(d, void search(const QString &searchTerm, SearchMode searchMode))
    Q_PRIVATE_SLOT(d, void centerMapOn(const QModelIndex &index))
    Q_PRIVATE_SLOT(d, void handlePlanetChange())

    SearchWidgetPrivate *const d;
};

}

#endif
