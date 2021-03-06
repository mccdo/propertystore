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
#define QT_NO_KEYWORDS

#include <qtpropertymanager.h>
#include <qteditorfactory.h>

#include <propertystore/GroupPropertyBrowser.h>

#include <iostream>

namespace propertystore
{

GroupPropertyBrowser::GroupPropertyBrowser(QWidget* parent) :
    QtGroupBoxPropertyBrowser(parent),
    mParser( 0 )
{
    // Create the default editor types we want to use
    mDoubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
    mSpinBoxFactory = new QtSpinBoxFactory(this);
    mCheckBoxFactory = new QtCheckBoxFactory(this);
    mLineEditFactory = new QtLineEditFactory(this);
    mComboBoxFactory = new QtEnumEditorFactory(this);
    mSliderFactory = new QtSliderFactory(this);

    // Set the type for mFileEditFactory. A similar procedure can be used to
    // set up other custom ExternalStringSelect types
    mFileEditFactory = new ExternalStringSelectFactory(this);
    mFileEditFactory->setEditorType( new FileEdit );

    mColorEditFactory = new ExternalStringSelectFactory(this);
    mColorEditFactory->setEditorType( new ColorEditRGBA );
}

GroupPropertyBrowser::~GroupPropertyBrowser()
{
    delete mParser;
}

void GroupPropertyBrowser::ParsePropertySet( PropertySetPtr set, bool autosize )
{
    if( !mParser )
    {
        SetPropertyParser( new PropertyParser );
    }

    mParser->SetDisplayUnitSystem( m_displayUnitSystem );
    mParser->ParsePropertySet( set );
    RefreshAllValues();
    RefreshContents( autosize );
}

void GroupPropertyBrowser::SetPropertyParser( PropertyParser* parser )
{
    if( mParser )
    {
        delete mParser;
    }
    mParser = parser;

    // Associate editor types with property types
    this->setFactoryForManager( parser->GetBoolManager(), mCheckBoxFactory );
    this->setFactoryForManager( parser->GetIntManager(), mSpinBoxFactory );
    this->setFactoryForManager( parser->GetDoubleManager(), mDoubleSpinBoxFactory );
    this->setFactoryForManager( parser->GetStringManager(), mLineEditFactory );
    this->setFactoryForManager( parser->GetEnumManager(), mComboBoxFactory );

    ExternalStringSelectManager* filePathManager = new ExternalStringSelectManager;
    parser->AddCustomExternalStringManager( filePathManager, "isFilePath" );
    this->setFactoryForManager( filePathManager, mFileEditFactory );

    ExternalStringSelectManager* colorManager = new ExternalStringSelectManager;
    parser->AddCustomExternalStringManager( colorManager, "isColorRGBA" );
    this->setFactoryForManager( colorManager, mColorEditFactory );

    connect( parser, SIGNAL(Modified()), SIGNAL(PropertySetModified()) );
}

void GroupPropertyBrowser::RefreshContents( bool autosize )
{
    // Clear out anything currently in the browser
    this->clear();

    // Tell the browser to auto-resize column widths
    if( autosize )
    {
        //this->setResizeMode(QtTreePropertyBrowser::ResizeToContents);
    }
    else
    {
        //this->setResizeMode(QtTreePropertyBrowser::Interactive);
        //this->setSplitterPosition( 175 );
    }

    // Get all the QItems from underlying propertybrowser and add to this view
    PropertyParser::ItemVector* items = mParser->GetItems();
    PropertyParser::ItemVector::iterator iterator;
    PropertyParser::ItemVector::iterator end = items->end();
    for( iterator = items->begin(); iterator != end; iterator++)
    {
        this->addProperty( (*iterator) );
    }

    mParser->RefreshAll( );

    // Auto-sizing of columns should be complete; allow user to control widths
    //this->setResizeMode(QtTreePropertyBrowser::Interactive);
}

void GroupPropertyBrowser::RefreshValues()
{
    if( mParser )
    {
        mParser->Refresh();
    }
}

void GroupPropertyBrowser::RefreshAllValues()
{
    if( mParser )
    {
        mParser->RefreshAll();
    }
}

void GroupPropertyBrowser::SetDisplayUnitSystem( DisplayUnitSystem system )
{
    m_displayUnitSystem = system;
}

}
