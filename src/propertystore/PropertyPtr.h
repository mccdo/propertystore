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

#include <propertystore/PointerTypes.h>

/**
 * \file propertystore::PropertyPtr
 *
 * Include this file to get a forward declaration of the pointer.
 * To get the full declaration of this pointer include the non-Ptr header file.
 */
 
/*!\class propertystore::PropertyPtr
 *
 */

/*!\namespace propertystore
 *
 */

namespace propertystore
{
class Property;
/// Typedef for a SmartPtr
typedef propertystore::ClassPtrDef<Property>::type  PropertyPtr;
typedef propertystore::SharedPtrDef<Property>::type PropertySharedPtr;
typedef propertystore::WeakPtrDef<Property>::type   PropertyWeakPtr;
typedef propertystore::ScopedPtrDef<Property>::type PropertyScopedPtr;
}
