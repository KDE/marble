//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_EXTERNALEDITORDIALOG_H
#define MARBLE_EXTERNALEDITORDIALOG_H

#include "marble_export.h"

#include <QtGui/QDialog>

#include "ui_ExternalEditor.h"

namespace Marble
{

class ExternalEditorDialogPrivate;

class MARBLE_EXPORT ExternalEditorDialog: public QDialog, private Ui::ExternalEditor
{
    Q_OBJECT

public:
    explicit ExternalEditorDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );

    ~ExternalEditorDialog();

    QString externalEditor() const;

    bool saveDefault() const;

private Q_SLOTS:
    void updateDefaultEditor( int index );

private:
    ExternalEditorDialogPrivate * const d;
};

} // namespace Marble

#endif // MARBLE_EXTERNALEDITORDIALOG_H
