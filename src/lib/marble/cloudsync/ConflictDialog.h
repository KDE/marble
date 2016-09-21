//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef CONFLICTDIALOG_H
#define CONFLICTDIALOG_H

#include "marble_export.h"

#include <QDialog>

class QDialogButtonBox;
class QAbstractButton;

namespace Marble
{

class MergeItem;

class MARBLE_EXPORT ConflictDialog : public QDialog
{
    Q_OBJECT

public:
    enum Button {
        Local = 1,
        Cloud,
        AllLocal,
        AllCloud
    };

    enum ResolveAction {
        AskUser,
        PreferLocal,
        PreferCloud
    };

    explicit ConflictDialog( QWidget *parent = 0 );
    void setMergeItem( MergeItem *item );

public Q_SLOTS:
    void open();
    void stopAutoResolve();

Q_SIGNALS:
    void resolveConflict( MergeItem *mergeItem );

private Q_SLOTS:
    void resolveConflict( QAbstractButton *button );

private:
    void prepareLayout();

    MergeItem *m_mergeItem;
    QDialogButtonBox *m_box;
    ConflictDialog::ResolveAction m_resolveAction;
};

}

#endif // CONFLICTDIALOG_H
