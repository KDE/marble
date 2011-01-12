//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_FILEVIEWMODEL_H
#define MARBLE_FILEVIEWMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QVariant>

#include "marble_export.h"

class QItemSelectionModel;

namespace Marble
{

class FileManager;

class MARBLE_EXPORT FileViewModel : public QAbstractListModel
{
    Q_OBJECT

  public:
    explicit FileViewModel( QObject* parent = 0 );
    ~FileViewModel();

    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual Qt::ItemFlags flags( const QModelIndex& index ) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

    void setFileManager( FileManager * fileManager );
    QItemSelectionModel * selectionModel();

  public slots:
    void saveFile();
    void closeFile();
    void append( int item );
    void remove( int item );

  private:
    Q_DISABLE_COPY( FileViewModel )
    QItemSelectionModel *m_selectionModel;
    FileManager *m_manager;
    
};

}

#endif
