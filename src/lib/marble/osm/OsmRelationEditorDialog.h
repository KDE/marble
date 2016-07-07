//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMRELATIONEDITORDIALOG_H
#define MARBLE_OSMRELATIONEDITORDIALOG_H

#include "marble_export.h"

#include <QDialog>

class QLineEdit;
class QDialogButtonBox;

namespace Marble {

class OsmRelationEditorDialogPrivate;
class OsmTagEditorWidget;
class OsmPlacemarkData;
class GeoDataPlacemark;

/**
 * @brief The OsmRelationEditorDialog class small dialog that is used to edit relations
 * The dialog has a line edit for name input, and a OsmTagEditorWidget for tag editing
 */
class MARBLE_EXPORT OsmRelationEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OsmRelationEditorDialog( OsmPlacemarkData *relationData, QWidget *parent = 0 );
    ~OsmRelationEditorDialog();

private Q_SLOTS:
    void checkFields();

private:
    void finish();
    friend class OsmRelationEditorDialogPrivate;
    QDialogButtonBox *m_buttonBox;
    QLineEdit *m_nameLineEdit;
    OsmTagEditorWidget *m_tagEditor;
    GeoDataPlacemark *m_dummyPlacemark;
    OsmPlacemarkData *m_relationData;
};

}

#endif
