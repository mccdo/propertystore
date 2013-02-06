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

#include <propertystore/Utilities.h>

#include <vector>
#include <sstream>
#include <iomanip>

namespace propertystore
{
////////////////////////////////////////////////////////////////////////////////
std::string SerializePropertySet( propertystore::PropertySetPtr mySet, bool showHidden )
{
    const std::vector<std::string> props = mySet->GetDataList();
    
    std::vector<std::string>::const_iterator itr = props.begin();
    std::ostringstream outputStream;
    while( itr != props.end() )
    {
        // Check whether property is set as visible before proceeding
        
        if( mySet->HasPropertyAttribute( *itr, "userVisible" ) )
        {
           if( !mySet->GetPropertyAttributeValue<bool>( *itr, "userVisible" ) && !showHidden )
           {
               ++itr;
               continue;
           }
        }

        crunchstore::DatumPtr datum = mySet->GetDatum( *itr );

        int displayPercision = 5;
        if( mySet->HasPropertyAttribute( *itr, "DisplayPrecision" ) )
        {
            displayPercision = mySet->GetPropertyAttributeValue<int>( *itr, "DisplayPrecision" );
        }

        // Print out the property name:
        //outputStream << *itr << ": ";
        outputStream << mySet->GetPropertyAttributeValue<std::string>( *itr, "uiLabel" ) << ": ";

        // Print out property contents:
        if( datum->IsBool() )
        {
           if( datum->extract<bool>() )
           {
               outputStream << "true";
           }
           else
           {
               outputStream << "false";
           }
        }
        else if( datum->IsInt() )
        {
           outputStream << datum->extract<int>();
        }
        else if( datum->IsFloat() )
        {
           outputStream << std::fixed << std::setprecision( displayPercision ) << datum->extract<float>();
        }
        else if( datum->IsDouble() )
        {
           outputStream << std::fixed << std::setprecision( displayPercision ) << datum->extract<double>();
        }
        else if( datum->IsString() )
        {
           outputStream << datum->extract<std::string>();
        }
        outputStream << std::endl;

        ++itr;
    }
    return outputStream.str();
}
////////////////////////////////////////////////////////////////////////////////
}
