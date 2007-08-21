#ifndef FILEVIEWMODEL_H
#define FILEVIEWMODEL_H

#include <QtCore/QAbstractListModel>

class FileViewModel : public QAbstractListModel
{
    Q_OBJECT

  public:
    FileViewModel( QObject* parent = 0 );

    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    void setSelectedIndex( const QModelIndex& index );

  public slots:
    void saveFile();
    void closeFile();

  private:
    QModelIndex m_selectedIndex;
};

#endif
