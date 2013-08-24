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
#include <propertystore/DeclarativePropertySet.h>
#include <propertystore/MakeLive.h>

#include <switchwire/OptionalMacros.h>

#include <iostream>
#include <vector>

void HideTestSlot( const std::string& uuid, const bool& value )
{
    std::cout << "HideTestSlot called with uuid: " << uuid << " and value: " << value << std::endl;
}

int main( int argc, char *argv[] )
{
    QApplication a(argc, argv);

    // Set up a slot to test live property signals
    switchwire::ScopedConnectionList connections;
    CONNECTSIGNALS_STATIC( "%HideVizFeature",
                            void( const std::string&, const bool& ),
                            &HideTestSlot,
                            connections,
                            any_SignalType,
                            normal_Priority );

    // Create a PropertySet and load its property schema from a file containing
    // json declarations
    propertystore::DeclarativePropertySetPtr ps( new propertystore::DeclarativePropertySet );
    ps->FromJSONFile( "JSONProp.json" );
    // The "Toggle Viz Off" property is setup as a delayed live property. It
    // won't be live until we make this call to EnableLiveProperties. Try
    // commenting out this call to see that the property never goes live.
    ps->EnableLiveProperties( true );

    // Create, set up, and show a GenericPropertyBrowser which can display
    // the PropertySet
    propertystore::GenericPropertyBrowser browserDisplay;
    browserDisplay.ParsePropertySet( ps );
    browserDisplay.resize( 350, 350 );
    browserDisplay.show();

    return a.exec();

}

