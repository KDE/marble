//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ECLIPSESBROWSERDIALOG_H
#define MARBLE_ECLIPSESBROWSERDIALOG_H

#include <QDialog>

namespace Ui {
    class EclipsesBrowserDialog;
}

namespace Marble {

class EclipsesModel;
class MarbleModel;

/**
 * @brief The eclipse browser dialog
 *
 * This implements the logic for the eclipse browser dialog.
 */
class EclipsesBrowserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EclipsesBrowserDialog( const MarbleModel *model,
                                    QWidget *parent = 0 );

    ~EclipsesBrowserDialog();

    /**
     * @brief Set the year
     *
     * This sets the year the browser currently shows eclipses for.
     *
     * @see year
     */
    void setYear( int year );

    /**
     * @brief Return the year the browser is set to
     *
     * @return The year the browser shows eclipses for at the moment
     * @see setYear
     */
    int year() const;

    /**
     * @brief Set whether or not to list lunar eclipses
     * @param enable whether or not to list lunar eclipses
     *
     * @see withLunarEclipses
     */
    void setWithLunarEclipses( const bool enable );

    /**
     * @brief Returns whether or not lunar eclipses are listed
     *
     * @return Whether or not lunar eclipses are listed
     * @see setWithLunarEclipses
     */
    bool withLunarEclipses() const;

Q_SIGNALS:
    /**
     * @brief This signal is emitted when the use clicks the "show" button
     * @param year the year of the selected eclipse event
     * @param index the index of the selected eclipse item
     */
    void buttonShowClicked( int year, int index );

    /**
     * @brief This signal is emitted when the 'Settings' button is clicked
     */
    void buttonSettingsClicked();

protected Q_SLOTS:
    /**
     * @brief Accept the dialog
     *
     * This emits the buttonShowClicked signal
     *
     * @see buttonShowClicked
     */
    void accept();

    /**
     * @brief Update the list of eclipses for the given year
     * @param year The year to list eclipses for
     */
    void updateEclipsesForYear( int year );

    /**
     * @brief Update the dialog's button states
     *
     * Disable/enable the show button according to the current selection.
     */
    void updateButtonStates();

protected:
    /**
     * @brief Initialize the object
     */
    void initialize();

private:
    const MarbleModel *m_marbleModel;
    Ui::EclipsesBrowserDialog *m_browserWidget;
    EclipsesModel *m_eclModel;
};

} // namespace Marble

#endif // MARBLE_ECLIPSESBROWSERDIALOG_H

