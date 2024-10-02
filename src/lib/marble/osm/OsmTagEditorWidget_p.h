// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMTAGEDITORWIDGETPRIVATE_H
#define MARBLE_OSMTAGEDITORWIDGETPRIVATE_H

#include "ui_OsmTagEditorWidget.h"

#include <QPair>

namespace Marble
{

class GeoDataPlacemark;
class OsmPlacemarkData;
class OsmTagEditorWidget;

class OsmTagEditorWidgetPrivate : public Ui::OsmTagEditorWidgetPrivate
{
public:
    using OsmTag = QPair<QString, QString>;
    OsmTagEditorWidgetPrivate();
    ~OsmTagEditorWidgetPrivate();
    void populatePresetTagsList();
    void populateCurrentTagsList();

private:
    friend class OsmTagEditorWidget;
    static QTreeWidgetItem *tagWidgetItem(const OsmTag &tag);

    /**
     * @brief recommendedTags builds a list of tags that are recommended by osm standards
     * taking the placemark's type and osmData in consideration.
     */
    QList<OsmTag> recommendedTags() const;

    /**
     * @brief generateTagFilter this is where the filter criteria for the recommended tags list
     * are set.
     */
    QStringList generateTagFilter() const;

    /**
     * @brief containsAny convenience function that determines whether the placemark contains
     * any of the given parameter tags
     */
    static bool containsAny(const OsmPlacemarkData &osmData, const QStringList &tags);

    /**
     * @brief addPattern convenience function that takes a list of OsmTags
     * and adds their keys to the filter, if the placemark doesn't already have them
     */
    static void addPattern(QStringList &filter, const OsmPlacemarkData &osmData, const QStringList &pattern);

    /**
     * @brief returns a list of useful, popular osm tags that currently don't have a visual category associated with them
     */
    static QList<OsmTag> createAdditionalOsmTags();

    GeoDataPlacemark *m_placemark;
    static const QString m_customTagAdderText;
};

}

#endif
