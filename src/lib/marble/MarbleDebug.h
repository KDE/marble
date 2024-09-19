// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_MARBLEDEBUG_H
#define MARBLE_MARBLEDEBUG_H

#include <QDebug>
#include <QLoggingCategory>

#include "marble_export.h"

#ifndef LOGGING_IDENTIFIER
#define LOGGING_IDENTIFIER MARBLE_DEFAULT
#endif
MARBLE_EXPORT Q_DECLARE_LOGGING_CATEGORY(LOGGING_IDENTIFIER)

    namespace Marble
{
    /**
      * A class which handles Marble debugging messages and settings.
      *
      * Use of this class is deprecated.  The preferred way to generate
      * debugging output within Marble is to use Qt categorised logging,
      * which allows control over the debugging output either via the
      * Qt configuration files and environment variables or the Plasma
      * @c kdebugsettings utility.

      * The @c mDebug() macro below logs debug messages under the category
      * named by @c LOGGING_CATEGORY or, if this is not specified, a
      * default.  In order to use a different logging category for a part
      * of the Marble source tree, define the logging category for ECM in
      * the @c CMakeLists.txt file for that part of the tree and then specify
      * the category to be used by the code, for example:
      *
      * @code
      * ecm_qt_export_logging_category(
      *   IDENTIFIER "MARBLE_SPECIAL"
      *   CATEGORY_NAME "marble_special"
      *   EXPORT marble
      *   DESCRIPTION "Marble (special category)")
      * add_definitions("-DLOGGING_IDENTIFIER=MARBLE_SPECIAL")
      * @endcode
      *
      * and then messages can be output by using @c qCDebug(MARBLE_SPECIAL)
      * if @c mDebug() is not being used.
      *
      * The logging identifier and category name must also be defined in
      * the @c MarbleDebug.cpp source file using @c Q_LOGGING_CATEGORY,
      * otherwise there will be undefined symbols which may not be resolved
      * until run time.
      *
      * ECM will collect together all of the logging definitions and install
      * the categories file to be use by @c kdebugsettings.
      *
      * @see QLoggingCategory
      */
    class MARBLE_EXPORT MarbleDebug
    {
    public:
        /**
         * @brief isEnabled returns whether debug information output is generated
         */
        static bool isEnabled();

        /**
         * @brief setEnabled Toggle debug information output generation
         * @param enabled Set to true to enable debug output, false to disable
         */
        static void setEnabled(bool enabled);

    private:
        static bool m_enabled;
    };

/**
 * A replacement for qDebug() in Marble library code.
 *
 * @note This cannot be a function (not even inline), because @c qCDebug()
 * is itself defined as a macro which captures the source location where
 * it is called.
 * @deprecated Use @c qCDebug() with a logging category
 */
#define mDebug() qCDebug(LOGGING_IDENTIFIER)

} // namespace Marble

#endif
