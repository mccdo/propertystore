/*************** <auto-copyright.rb BEGIN do not edit this line> **************
 *
 * VE-Suite is (C) Copyright 1998-2012 by Iowa State University
 *
 * Original Development Team:
 *   - ISU's Thermal Systems Virtual Engineering Group,
 *     Headed by Kenneth Mark Bryden, Ph.D., www.vrac.iastate.edu/~kmbryden
 *   - Reaction Engineering International, www.reaction-eng.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * -----------------------------------------------------------------
 * Date modified: $Date$
 * Version:       $Rev$
 * Author:        $Author$
 * Id:            $Id$
 * -----------------------------------------------------------------
 *
 *************** <auto-copyright.rb END do not edit this line> ***************/
#pragma once

#define QT_NO_KEYWORDS

#include <QtCore/QObject>
#include <qtpropertybrowser.h>
#include <qtpropertymanager.h>
#include <qteditorfactory.h>

#include <PropertySetBrowser/ExternalStringSelectFactory.h>
#include <PropertySetBrowser/ExternalStringSelectManager.h>
#include <PropertySetBrowser/FileEdit.h>
#include <PropertySetBrowser/PropertySet.h>
#include <PropertySetBrowser/Property.h>
#include <PropertySetBrowser/Logging.h>

#include <vector>

namespace PropertySetBrowser
{
/// @file PropertyParser.h

/// @class PropertySetBrowser::PropertyParser
/// Class that knows how to parse a property set and make it usable by classes
/// derived from QtAbstractPropertyBrowser. The class maintains data
/// synchronicity between the UI generated by QtAbstractPropertyBrowser and the
/// underlying data set (represented via an instance of PropertySet).
/// Visual hierarchies can be created in the the PropertySet by giving the
/// members names separated by underscores to indicate relationship; eg. the
/// Property named "Axes_XLabel" will be treated visually as a sub-property
/// of the Property names "Axes". Likewise, "Axes_XLabel_Font" will be treated
/// as a sub-property of "Axes_XLabel".

class PROPERTYSETBROWSER_EXPORT PropertyParser : public QObject
{
    Q_OBJECT

public:
    typedef std::vector< PropertyPtr > PropertyVector;
    typedef std::vector< QtProperty* > ItemVector;

    ///
    /// Constructor
    PropertyParser( QObject* parent = 0 );

    ///
    /// Destructor
    ~PropertyParser();

    ///
    /// Returns a list of all QtProperty this browser instance knows about
    ItemVector* GetItems();

    ///
    /// Parse the PropertySet passed as set.
    /// This will set up everything that an instance of GenericPropertyBrowser
    /// needs to create an interface and interact with the underlying
    /// PropertySet.
    void ParsePropertySet( PropertySetPtr set );

    ///
    /// Tells the parser to request a list of changes (values, enabled state,
    /// min/max, etc.) from the underlying PropertySet and update the UI
    /// accordingly.
    void Refresh();

    ///
    /// Tells the parser to re-read the entire underlying PropertySet and
    /// reset everything in the UI. This differs from Refresh() in that
    /// the parser does not ask the PropertySet for a list of things that have
    /// changed; instead it just blindly re-reads the entire set and is
    /// consequently a more costly operation.
    void RefreshAll();

    ///
    /// Returns the manager used for double types
    QtDoublePropertyManager* GetDoubleManager();

    ///
    /// Returns the manager used for string types
    QtStringPropertyManager* GetStringManager();

    ///
    /// Returns the manager used for bool types
    QtBoolPropertyManager* GetBoolManager();

    ///
    /// Returns the manager used for enumerated types
    QtEnumPropertyManager* GetEnumManager();

    ///
    /// Returns the manager used for int types
    QtIntPropertyManager* GetIntManager();

    ///
    /// Returns the manager used for group types
    QtGroupPropertyManager* GetGroupManager();

    /** Adds a custom manager for string properties having a boolean attribute
      * named the same as the string passed in attributeFlag and set to true.
      * The manager passed as the first argument should be an instance of
      * ExternalStringSelectManager that has been set up with an instance of
      * ExternalStringSelectFactory, which in turn has been given an instance of
      * your own custom type derived from ExternalStringSelect.
      *
      * This functionality allows you to set up custom ways of selecting a
      * string into a string property -- such as custom filesystem browsers,
      * scenegraph node selection browsers, password browsers, etc.
    **/
    void AddCustomExternalStringManager( ExternalStringSelectManager* manager, const std::string& attributeFlag );


Q_SIGNALS:
    public Q_SLOTS :
    ///
    /// Internal slot that catches valueChanged signal from QtProperty bool types
    /// and attempts to synchronize the value of the underlying Property
    void BoolValueChanged( QtProperty* item, bool value );

    ///
    /// Internal slot that catches valueChanged signal from QtProperty int and enum types
    /// and attempts to synchronize the value of the underlying Property
    void IntValueChanged( QtProperty* item, int value );

    ///
    /// Internal slot that catches valueChanged signal from QtProperty double types
    /// and attempts to synchronize the value of the underlying Property
    void DoubleValueChanged( QtProperty* item, double value );

    ///
    /// Internal slot that catches valueChanged signal from QtProperty string types
    /// and attempts to synchronize the value of the underlying Property
    void StringValueChanged( QtProperty* item, const QString & value );


private:
    // The following three vectors should be synchronized at all times.
    // 1. They should always have same number of items.
    // 2. The name at position X in mPropertyNames must be the name of the
    //    Property at position X in mProperties.
    // 3. The Property at position X must be the underlying data for the
    //    QtProperty at position X in mItems.
    PropertyVector mProperties; // Holds all instances of Property
    ItemVector mItems; // Holds all instances of QtProperty (including subproperties)
    PropertySet::PSVectorOfStrings mPropertyNames; // Holds the names of all the properties

    ItemVector mTreedItems; // Holds only top-level instances of QtProperty
    // (ie. does not explictly hold sub-properties)

    PropertySetPtr mSet; // Pointer to underlying PropertySet ( the real data )

    QtDoublePropertyManager* mDoubleManager;
    QtStringPropertyManager* mStringManager;
    QtBoolPropertyManager* mBooleanManager;
    QtEnumPropertyManager* mEnumManager;
    QtGroupPropertyManager* mGroupManager;
    QtIntPropertyManager* mIntManager;
    std::map< std::string, ExternalStringSelectManager* > m_customStringManagers;

    // Helper functions to find things in the triad of synchronized vectors
    int _getPropertyIndex( PropertyPtr property );
    int _getItemIndex( QtProperty* item );
    int _getPropertyIndexByName( std::string name );

    /// Set the underlying Property to the value of the QtProperty
    void _setPropertyValue( QtProperty* item, boost::any value );

    /// Set the QtProperty to the value of the underlying Property
    void _setItemValue( QtProperty* item, PropertyPtr property );

    /// Helper function to do the dirty work of finding out if a Property has
    /// min and max values
    void _extractMinMaxValues( PropertyPtr property, double* min,
                               double* max, bool* hasMin, bool* hasMax );

    /// Put QtProperty instances into a hierarchical relationship as requested
    /// by the name of the underlying Property
    void _createHierarchy();

    /// Helper function to refresh the UI value of a specific Property/QtProperty
    void _refreshItem( int index );

    /** When this is true, changes to values in the browser are not pushed down
      * to the underlying PropertySet.
      *
      * This is needed when doing operations on
      * browser items that have automatic side-effects that alter the item's
      * value. (Eg. Changing the range of a numeric item automatically forces
      * the value to the closest min/max if it happens to be outside of the range
      * when the range is changed. We don't want to push this value down into the
      * PropertySet because the PropertySet has already done this sort of operation
      * on its values, and may have changed the value to something other than the
      * closest min/max.)
      **/
    bool m_ignoreValueChanges;

    ///Logger reference
    Poco::Logger& m_logger;
    ///Log stream for this class
    LogStreamPtr m_logStream;
};

} // namespace
