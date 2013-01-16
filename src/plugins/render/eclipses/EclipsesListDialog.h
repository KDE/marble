//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ECLIPSESLISTDIALOG_H
#define MARBLE_ECLIPSESLISTDIALOG_H

#include <QDialog>
#include <QTreeView>
#include <QListWidget>
#include <QListWidgetItem>

namespace Ui {
    class EclipsesListDialog;
}

namespace Marble {

class EclipsesModel;
class MarbleModel;

class EclipsesListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EclipsesListDialog( const MarbleModel *model,
                                 QWidget *parent = 0 );
    ~EclipsesListDialog();

    void setYear( int year );
    int year() const;

Q_SIGNALS:
    void buttonShowEclipseClicked( int year, int index );
    void buttonSettingsClicked();

protected Q_SLOTS:
    void accept();
    void updateEclipsesListForYear( int year );

protected:
    void initialize();

private:
    const MarbleModel *m_marbleModel;
    Ui::EclipsesListDialog *m_listWidget;
    EclipsesModel *m_eclModel;
};

} // namespace Marble

#endif // MARBLE_ECLIPSESLISTDIALOG_H

