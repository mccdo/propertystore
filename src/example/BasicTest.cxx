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
#include <QtGui/QApplication>

#include <propertystore/GenericPropertyBrowser.h>
#include <propertystore/PropertySet.h>

#include "TestSet.h"

#include <crunchstore/SQLiteStore.h>
#include <crunchstore/NullBuffer.h>
#include <crunchstore/NullCache.h>
#include <crunchstore/DataManager.h>

#include <iostream>
#include <cmath>

bool evenValidator( propertystore::PropertyPtr, boost::any value )
{
    // Only allow even numbers
    int castValue = boost::any_cast<int>(value);
    if( castValue % 2 == 0 )
    {
        std::cout << "Allowing value" << std::endl << std::flush;
        return true;
    }
    std::cout << "Disallowing value" << std::endl << std::flush;
    return false;
}

void printIndex( propertystore::PropertyPtr property )
{
    std::cout << "AnEnum index = " << property->GetAttribute("enumCurrentIndex")->extract<int>() << std::endl << std::flush;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    // Create a PropertySet with some test data
    propertystore::PropertySetPtr propertySet( new propertystore::PropertySet );
    propertySet->SetTypeName( "MyTest" );

    propertySet->AddProperty( "ABoolean", true, "A Boolean Value" );
    propertySet->AddProperty( "ABoolean_Sub1", false, "Subvalue 1" );
    propertySet->AddProperty( "ABoolean_Sub2", 99, "Subvalue 2" );
    propertySet->AddProperty( "ABoolean_Sub3", 1.0134, "Subvalue 3" );

    propertySet->AddProperty( "AnInteger", 44, "An Integer value" );
    propertySet->SetPropertyAttribute( "AnInteger", "minimumValue", 41 );
    propertySet->SetPropertyAttribute( "AnInteger", "maximumValue", 50 );

    propertySet->AddProperty( "ADouble", 3.14, "A Double value" );

    propertySet->AddProperty( "AString", std::string("Hello World"), "A String Value" );

    propertySet->AddProperty( "AnEnum", std::string(), "An Enumerated Value" );
    std::vector< std::string > choices;
    choices.push_back("Option 1");
    choices.push_back("Option 2");
    choices.push_back("Option 3");
    propertySet->SetPropertyAttribute( "AnEnum", "enumValues", choices );
    propertySet->GetProperty( "AnEnum" )->SignalValueChanged.connect( &printIndex );

    propertySet->AddProperty( "AFilePath", std::string("Choose a file..."), "Path to some file" );
    propertySet->SetPropertyAttribute( "AFilePath", "isFilePath", true );

    propertySet->AddProperty( "IntWithValidation", 2, "Even-only integer" );
    propertySet->GetProperty( "IntWithValidation" )->SignalRequestValidation.connect( &evenValidator );
    // If the validator is a member method of a class, you have to use boost::bind
    // in the call to connect, like this:
    // propertySet->GetProperty( "IntWithValidation" )->
    //     SignalRequestValidation.connect(
    //     boost::bind( &MyClassName::MyMemberMethodName, this, _1, _2 ) );

    // Test the copy constructor. Notice we have to de-reference the propertySet
    // pointer to invoke the copy constructor.
    propertystore::PropertySetPtr copiedSet( new propertystore::PropertySet( *(propertySet) ) );
    std::cout << "Copy constructor test:" << std::endl;
    std::cout << "            UUID                                  AnInteger    min" << std::endl;
    std::cout << "  Original: " << propertySet->GetUUIDAsString() << "    " << propertySet->GetDatumValue< int >( "AnInteger" )
              << "         " << propertySet->GetPropertyAttributeValue< int >( "AnInteger", "minimumValue" ) << std::endl;
    std::cout << "  Copy    : " << copiedSet->GetUUIDAsString() << "    " << copiedSet->GetDatumValue< int >( "AnInteger" )
              << "         " << copiedSet->GetPropertyAttributeValue< int >( "AnInteger", "minimumValue" ) << std::endl << std::endl;

    // Set up an SQLiteStore and save propertySet out to it.
    crunchstore::DataManagerPtr manager( new crunchstore::DataManager );
    crunchstore::DataAbstractionLayerPtr cache( new crunchstore::NullCache );
    crunchstore::DataAbstractionLayerPtr buffer( new crunchstore::NullBuffer );
    crunchstore::SQLiteStorePtr store( new crunchstore::SQLiteStore );
    manager->SetCache( cache );
    manager->SetBuffer( buffer );
    store->SetStorePath( "/tmp/basicTest.db" );
    manager->AttachStore( store, crunchstore::Store::WORKINGSTORE_ROLE );

    propertySet->SetDataManager( manager );
    propertySet->Save();

    // Remember the uuid, then change that and the value of AnInteger on
    // propertySet. Then we'll do a LoadByKey, where the key is uuid and the
    // value is the old uuid. This will check both whether the data made it into
    // the store as well as whether LoadByKey works.
    std::string uuid = propertySet->GetUUIDAsString();
    propertySet->SetUUID( "00000000-0101-1010-1111-000011110000" );
    propertySet->SetPropertyValue( "AnInteger", 88 );
    propertySet->LoadByKey( "uuid", uuid );
    std::cout << "If LoadByKey worked, this value will be 44: " << std::flush;
    std::cout << propertySet->GetDatumValue< int >( "AnInteger" )
              << std::endl;

    // Create, set up, and show a GenericPropertyBrowser which can display
    // the PropertySet
    propertystore::GenericPropertyBrowser browserDisplay;
    browserDisplay.ParsePropertySet( propertySet );
    browserDisplay.resize( 350, 350 );
    browserDisplay.show();
    
    return a.exec();
}
