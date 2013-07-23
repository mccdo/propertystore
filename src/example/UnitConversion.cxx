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
#include <propertystore/UnitConverter.h>
#include <propertystore/MakeLive.h>

#include <switchwire/EventManager.h>
#include <switchwire/OptionalMacros.h>

// logging facilities.
#include <Poco/Logger.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>

#include <iostream>

void printValue( const std::string& uuid, double value )
{
    std::cout << "Signal rec'd; value changed to " << value << std::endl;
}

int main(int argc, char *argv[])
{

    // Set up a logger so we can see log messages from the propertystore library
    Poco::Logger::root().setLevel("information");
    Poco::ConsoleChannel* console = new Poco::ConsoleChannel;
    Poco::PatternFormatter* formatter = new Poco::PatternFormatter;
    formatter->setProperty("pattern", "%s: %t");
    Poco::FormattingChannel* formattingChannel =
            new Poco::FormattingChannel( formatter, console );
    Poco::Logger::root().setChannel( formattingChannel );

    //propertystore::UnitConverter converter;
    //std::cout << converter.Convert( 2.0, "meter", "foot" ) << std::endl;

    QApplication a(argc, argv);

    // Create a PropertySet with some test data
    propertystore::PropertySetPtr propertySet( new propertystore::PropertySet );
    propertySet->SetTypeName( "UnitConversionTest" );

    propertySet->AddProperty( "ADouble", 2.0, "A Double value" );
    // Set up the storage unit system. Valid values are "SI" and "US"
    propertySet->SetPropertyAttribute( "ADouble", "StorageUnitSystem", std::string("US") );
    propertySet->SetPropertyAttribute( "ADouble", "SIDisplayUnit", std::string("meter") );
    propertySet->SetPropertyAttribute( "ADouble", "USDisplayUnit", std::string("foot") );
    propertySet->SetPropertyAttribute( "ADouble", "USDisplayLabel", std::string("A Double value (ft)") );
    propertySet->SetPropertyAttribute( "ADouble", "SIDisplayLabel", std::string("A Double value (m)") );

    // A second double value with no units attached
    propertySet->AddProperty( "DoubleTwo", 4.0, "2nd Double (no units)" );

    propertystore::MakeLiveBasePtr p = propertystore::MakeLiveBasePtr(
         new propertystore::MakeLive<double>( propertySet->GetUUIDAsString(),
             propertySet->GetProperty("ADouble"),
             "ADoubleSignal") );

    propertystore::MakeLiveBasePtr q = propertystore::MakeLiveBasePtr(
         new propertystore::MakeLive<double>( propertySet->GetUUIDAsString(),
             propertySet->GetProperty("DoubleTwo"),
             "ADoubleSignal") );

    switchwire::ScopedConnectionList connections;
    CONNECTSIGNALS_STATIC( "%ADoubleSignal",
                            void( const std::string&, double ),
                            &printValue,
                            connections,
                            any_SignalType,
                            normal_Priority );

    // Create, set up, and show a GenericPropertyBrowser which can display
    // the PropertySet
    propertystore::GenericPropertyBrowser browserDisplay;
    // Set up the display unit system for this browser. Valid values are
    // SI_Units and US_Units
    if( argc > 1 )
    {
        std::string sys( argv[1] );
        if( sys == "SI" )
        {
            browserDisplay.SetDisplayUnitSystem( propertystore::SI_Units );
        }
        else
        {
            browserDisplay.SetDisplayUnitSystem( propertystore::US_Units );
        }
    }
    else
    {
        browserDisplay.SetDisplayUnitSystem( propertystore::US_Units );
    }
    browserDisplay.resize( 400, 350 );
    browserDisplay.ParsePropertySet( propertySet );
    browserDisplay.show();

    return a.exec();
}

