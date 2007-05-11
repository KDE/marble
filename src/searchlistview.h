#ifndef SEARCHLISTVIEW_H
#define SEARCHLISTVIEW_H


#include <QtGui/QListView>


class SearchListView : public QListView
{
    Q_OBJECT

 public:
    SearchListView(QWidget*);

 signals:
    void centerOn(const QModelIndex&);

 public slots:
    void  selectItem(QString);
    void  activate() {
        if ( selectedIndexes().size() > 0 )
            emit activated( currentIndex() );
    }
};

#endif // SEARCHLISTVIEW_H
