// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMRELATIONEDITORDIALOG_H
#define MARBLE_OSMRELATIONEDITORDIALOG_H

#include "marble_export.h"

#include <QDialog>

class QLineEdit;
class QDialogButtonBox;

namespace Marble
{

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
    explicit OsmRelationEditorDialog(OsmPlacemarkData *relationData, QWidget *parent = nullptr);
    ~OsmRelationEditorDialog() override;

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
