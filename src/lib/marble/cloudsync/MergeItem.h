// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef MERGEITEM_H
#define MERGEITEM_H

#include "GeoDataPlacemark.h"
#include "marble_export.h"

#include <QObject>

namespace Marble
{

class MARBLE_EXPORT MergeItem : public QObject
{
    Q_OBJECT

public:
    MergeItem();

    Q_PROPERTY(QString pathA READ pathA WRITE setPathA NOTIFY pathAChanged)
    Q_PROPERTY(QString pathB READ pathB WRITE setPathB NOTIFY pathBChanged)
    Q_PROPERTY(GeoDataPlacemark placemarkA READ placemarkA WRITE setPlacemarkA NOTIFY placemarkAChanged)
    Q_PROPERTY(GeoDataPlacemark placemarkB READ placemarkB WRITE setPlacemarkB NOTIFY placemarkBChanged)
    Q_PROPERTY(QString nameA READ nameA NOTIFY nameAChanged())
    Q_PROPERTY(QString nameB READ nameB NOTIFY nameBChanged())
    Q_PROPERTY(QString descriptionA READ descriptionA NOTIFY descriptionAChanged())
    Q_PROPERTY(QString descriptionB READ descriptionB NOTIFY descriptionBChanged())
    Q_PROPERTY(MergeItem::Action actionA READ actionA WRITE setActionA NOTIFY actionAChanged)
    Q_PROPERTY(MergeItem::Action actionB READ actionB WRITE setActionB NOTIFY actionBChanged)
    Q_PROPERTY(MergeItem::Resolution resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)

    enum Resolution {
        None,
        A,
        B
    };

    enum Action {
        Changed,
        Deleted
    };

    QString pathA() const;
    void setPathA(const QString &path);

    QString pathB() const;
    void setPathB(const QString &path);

    GeoDataPlacemark placemarkA() const;
    void setPlacemarkA(const GeoDataPlacemark &placemark);

    GeoDataPlacemark placemarkB() const;
    void setPlacemarkB(const GeoDataPlacemark &placemark);

    QString nameA() const;
    QString nameB() const;

    QString descriptionA() const;
    QString descriptionB() const;

    MergeItem::Action actionA() const;
    void setActionA(MergeItem::Action action);

    MergeItem::Action actionB() const;
    void setActionB(MergeItem::Action action);

    MergeItem::Resolution resolution() const;
    void setResolution(MergeItem::Resolution resolution);

Q_SIGNALS:
    void pathAChanged();
    void pathBChanged();
    void placemarkAChanged();
    void placemarkBChanged();
    void nameAChanged();
    void nameBChanged();
    void descriptionAChanged();
    void descriptionBChanged();
    void actionAChanged();
    void actionBChanged();
    void resolutionChanged();

private:
    QString m_pathA;
    QString m_pathB;
    GeoDataPlacemark m_placemarkA;
    GeoDataPlacemark m_placemarkB;
    MergeItem::Action m_actionA;
    MergeItem::Action m_actionB;
    MergeItem::Resolution m_resolution;
};

}

#endif // MERGEITEM_H
