//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMTAGEDITORWIDGETPRIVATE_H
#define MARBLE_OSMTAGEDITORWIDGETPRIVATE_H

#include "ui_OsmTagEditorWidget.h"
#include "osm/OsmPresetLibrary.h"

namespace Marble
{
class OsmTagEditorWidget;


class OsmTagEditorWidgetPrivate : public Ui::OsmTagEditorWidgetPrivate
{
public:
    typedef OsmPresetLibrary::OsmTag OsmTag;
    OsmTagEditorWidgetPrivate();
    ~OsmTagEditorWidgetPrivate();
    void populatePresetTagsList();
    void populateCurrentTagsList();

private:
    friend class OsmTagEditorWidget;
    QTreeWidgetItem *tagWidgetItem( const OsmTag &tag ) const;

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
    bool containsAny( const OsmPlacemarkData &osmData, const QStringList &tags ) const;

    /**
     * @brief addPattern convenience function that takes a list of OsmTags
     * and adds their keys to the filter, if the placemark doesn't already have them
     */
    void addPattern( QStringList &filter, const OsmPlacemarkData &osmData, const QStringList &pattern ) const;

    GeoDataPlacemark *m_placemark;
    static const QString m_customTagAdderText;
};

}

#endif
