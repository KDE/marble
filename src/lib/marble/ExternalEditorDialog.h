// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_EXTERNALEDITORDIALOG_H
#define MARBLE_EXTERNALEDITORDIALOG_H

#include "marble_export.h"

#include <QDialog>

#include "ui_ExternalEditor.h"

namespace Marble
{

class ExternalEditorDialogPrivate;

class MARBLE_EXPORT ExternalEditorDialog : public QDialog, private Ui::ExternalEditor
{
    Q_OBJECT

public:
    explicit ExternalEditorDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    ~ExternalEditorDialog() override;

    QString externalEditor() const;

    bool saveDefault() const;

private Q_SLOTS:
    void updateDefaultEditor(int index);

private:
    ExternalEditorDialogPrivate *const d;
};

} // namespace Marble

#endif // MARBLE_EXTERNALEDITORDIALOG_H
