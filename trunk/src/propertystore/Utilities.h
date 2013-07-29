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

#include <propertystore/PropertySet.h>

#include <string>

namespace propertystore
{
///Dump a propertyset to a string
PROPERTYSTORE_EXPORT std::string SerializePropertySet( propertystore::PropertySetPtr mySet, bool showHidden = false );

PROPERTYSTORE_EXPORT void AddUnits( propertystore::PropertySetPtr propertySet,
                                    const std::string& propertyName,
                                    const std::string& storageUnitSystem,
                                    const std::string& SIDisplayUnit,
                                    const std::string& SIDisplayLabel,
                                    const std::string& USDisplayUnit,
                                    const std::string& USDisplayLabel );

PROPERTYSTORE_EXPORT void AddUnits( propertystore::PropertyPtr property,
                                    const std::string& storageUnitSystem,
                                    const std::string& SIDisplayUnit,
                                    const std::string& SIDisplayLabel,
                                    const std::string& USDisplayUnit,
                                    const std::string& USDisplayLabel );
}
