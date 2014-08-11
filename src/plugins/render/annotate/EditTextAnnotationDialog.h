//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru <crucerucalincristian@gmail.com>
//

#ifndef EDITPLACEMARKDIALOG_H
#define EDITPLACEMARKDIALOG_H

#include <QDialog>


namespace Marble {

class GeoDataFeature;
class GeoDataPlacemark;
class PlacemarkTextAnnotation;


/**
 * @brief The EditTextAnnotationDialog class deals with customizing placemarks.
 */
class EditTextAnnotationDialog : public QDialog
{
    Q_OBJECT

public:
    EditTextAnnotationDialog( PlacemarkTextAnnotation *textAnnotation,
                              QWidget *parent = 0 );
    ~EditTextAnnotationDialog();

    /**
     * @brief setFirstTimeEditing tells the dialog whether this is the first time it
     * pops up for this PlacemarkTextAnnotation instance or not.
     */
    void setFirstTimeEditing( bool enabled );

public slots:
    /**
     * @brief updateDialogFields is connected to a signal from AnnotatePlugin in order
     * to update some fields in the dialog as the user interacts directly with the text
     * annotation item.
     */
    void updateDialogFields();

private slots:
    /**
     * @brief loadIconFile deals with showing the file dialog and making sure only
     * supported icon files are loaded.
     */
    void loadIconFile();

    /**
     * @brief checkFields shows warnings if there are important fields which don't hold
     * accurate information.
     */
    void checkFields();

    /**
     * @brief updateTextAnnotation is the main slot which synchronizes the information
     * from the dialog with the way the text annotation item is painted.
     */
    void updateTextAnnotation();

    /**
     * @brief updateLabelDialog The color chooser for label is represented as a push
     * button with a filled pixmap as its icon. This slot updates the color fill of this
     * pixmap.
     */
    void updateLabelDialog( const QColor &color );

    /**
     * @brief updateIconDialog The same as above, but for icons.
     * FIXME: This is not functional ATM - we need some mechanism for customizing existing
     * icons.
     */
    void updateIconDialog( const QColor &color );

    /**
     * @brief restoreInitial restores the settings before the dialog has been shown. In case
     * this is the first time the dialog is shown for this text annotation instance, the
     * text annotation graphic item gets removed (a signal is emitted which should be listened
     * to in AnnotatePlugin).
     */
    void restoreInitial();

signals:
    /**
     * @brief textAnnotationUpdated signals that some property of the PlacemarkTextAnnotation
     * instance has changed.
     * @param feature The instance's placemark.
     */
    void textAnnotationUpdated( GeoDataFeature *feature );

    /**
     * @brief removeRequested Is the signal emitted when this is the first time for this
     * PlacemarkTextAnnotation instance the dialog pops up and the close button is pressed.
     */
    void removeRequested( PlacemarkTextAnnotation *targetedPlacemark );

private:
    class Private;
    Private * const d;
};

}

#endif
