// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
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

    explicit ConflictDialog(QWidget *parent = nullptr);
    void setMergeItem(MergeItem *item);

public Q_SLOTS:
    void open() override;
    void stopAutoResolve();

Q_SIGNALS:
    void resolveConflict(MergeItem *mergeItem);

private Q_SLOTS:
    void resolveConflict(QAbstractButton *button);

private:
    void prepareLayout();

    MergeItem *m_mergeItem = nullptr;
    QDialogButtonBox *m_box = nullptr;
    ConflictDialog::ResolveAction m_resolveAction;
};

}

#endif // CONFLICTDIALOG_H
