#ifndef SEARCHCOMBOBOX_H
#define SEARCHCOMBOBOX_H


#include <QtGui/QComboBox>


class SearchComboBox : public QComboBox
{
    Q_OBJECT

 public:
    SearchComboBox(QWidget*);

 public slots:
    void showListView(QString);
};


#endif // SEARCHCOMBOBOX_H
