/*************** <auto-copyright.rb BEGIN do not edit this line> **************
 *
 * Copyright 2012-2012 by Ames Laboratory
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2.1 as published by the Free Software Foundation.
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
 *************** <auto-copyright.rb END do not edit this line> ***************/
#pragma once

#define QT_NO_KEYWORDS

#include <qtgroupboxpropertybrowser.h>
#include <propertystore/PropertyParser.h>

/// @file GroupPropertyBrowser.h
/// @namespace propertystore
/// @class GroupPropertyBrowser ties together a QtTreePropertyBrowser,
/// a set of editor factories, and a PropertySetParser. Use it to display
/// a tree-type property browser. If you need a different type ( such as a
/// groupbox browser ) or need to handle custom string types -- this one handles
/// file paths as an example of a custom string type -- the best approach is to
/// copy all the code from this class into a new class and then alter it to
/// create the appropriate factories, do any resizing or polish needed, etc.

namespace propertystore
{

class PROPERTYSTORE_EXPORT GroupPropertyBrowser : public QtGroupBoxPropertyBrowser
{
Q_OBJECT
public:
    explicit GroupPropertyBrowser(QWidget* parent = 0);

    virtual ~GroupPropertyBrowser();

    void SetPropertyParser( PropertyParser* parser );

    /// Refreshes the list of properties to be displayed. This does not refresh
    /// the *values* of the property-value pairs, but only refreshes the property
    /// labels and the value types.
    /// @param autosize When true autosizes the column widths to attempt to
    ///                 display all information. When false, renders the property
    ///                 and value columns as equal widths.
    void RefreshContents( bool autosize = true );

    void ParsePropertySet( PropertySetPtr set, bool autosize = true );

    /// Refreshes only values of property-value pairs that have changed.
    void RefreshValues();

    /// Re-reads values of all property-value pairs. More expensive operation
    /// than RefreshValues();
    void RefreshAllValues();

Q_SIGNALS:

public Q_SLOTS:

private:
    PropertyParser* mParser;
    QtDoubleSpinBoxFactory* mDoubleSpinBoxFactory;
    QtSpinBoxFactory* mSpinBoxFactory;
    QtCheckBoxFactory* mCheckBoxFactory;
    QtLineEditFactory* mLineEditFactory;
    QtEnumEditorFactory* mComboBoxFactory;
    QtSliderFactory* mSliderFactory;
    ExternalStringSelectFactory* mFileEditFactory;
    //ExternalStringSelectFactory* mNodeSelectFactory;
};

} // namespace
