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
#include <map>
#include <string>
#include <vector>

#include <boost/any.hpp>
#include <boost/uuid/uuid.hpp>

#include <Poco/Types.h>

#include <crunchstore/Persistable.h>
#include <crunchstore/DataManagerPtr.h>

#include <propertystore/Exports.h>
#include <propertystore/MakeLivePtr.h>
#include <propertystore/PropertySetPtr.h>
#include <propertystore/PropertyPtr.h>
#include <propertystore/Logging.h>

// Forward declarations
namespace Poco
{
class Timer;
namespace Data
{
class Session;
class Statement;
} // namespace Data
} // namespace Poco

namespace propertystore
{
/*!\file PropertySet.h
 * \class propertystore::PropertySet
 * PropertySet is a base class for collections of properties
 * containing methods to read and write values and attributes of
 * properties, as well as to get information about changes to other properties
 * that may occur when a property value or attribute is changed.
 * \namespace propertystore
 *
 */

class PROPERTYSTORE_EXPORT PropertySet: public crunchstore::Persistable
{
public:
    typedef std::vector<std::string> PSVectorOfStrings;


    /// Constructor
    PropertySet();


    /// Copy constructor
    PropertySet( const PropertySet& orig );


    /// Destructor
    virtual ~PropertySet();

    /// Method to allow factory creation. Derived classes must override this
    /// to return an object of the derived type wrapped in a PropertySetPtr.
    virtual PropertySetPtr CreateNew();


    /// Add a new property named propertyName with main value value to this set
    virtual void AddProperty( const std::string& propertyName,
                              boost::any value,
                              std::string uiLabel = "" );

    /// Sets the value of @c propertyName to @c value. Synonymous with
    /// SetDatumValue().
    virtual bool SetPropertyValue( const std::string& propertyName,
                                   boost::any value );

    /// Returns the underlying boost::any value of property @c propertyName
    /// @throws None Does not throw. Contrast this with GetProperty, which throws
    /// if the requested property does not exist. GetPropertyValue returns an
    /// empty boost::any if the property does not exist.
    virtual boost::any GetPropertyValue( const std::string& propertyName );

    /// Returns a @c PropertyPtr for the property named @c propertyName
    /// @throws std::runtime_error if property does not exist.
    virtual PropertyPtr GetProperty( const std::string& propertyName );

    /// Returns whether this property exists. Synonymous with DatumExists().
    virtual bool PropertyExists( std::string const& propertyName ) const;

    /// Returns a vector containing the identifying names of all attributes
    /// owned by property propertyName
    virtual const PSVectorOfStrings& GetPropertyAttributeList( std::string const&
                                                            propertyName );


    /// Returns value of the property attribute identified by
    /// attributeName and owned by property propertyName
    /// @throws std::runtime_error if property does not exist
    virtual crunchstore::DatumPtr GetPropertyAttribute( std::string const& propertyName,
                                             std::string const& attributeName ) const;

    /// Returns the value of the attribute identified by attributeName of the
    /// property identified by propertName, converted
    /// to type T. Example: propertySet.GetPropertyAttributeValue< double >
    ///                                  ( "SomeProperty", "ADoubleAttribute" );
    template <typename T>
            T GetPropertyAttributeValue( std::string const& propertyName,
                                         std::string const& attributeName ) const
    {
        T val = GetPropertyAttribute( propertyName, attributeName )->extract<T>();
        return val;
    }


    /// Returns true if the property with name propertyName is currently enabled.
    virtual bool GetPropertyEnabled( std::string const& propertyName ) const;


    /// Sets the property attribute given by attributeName of the property
    /// propertyName to value.
    virtual void SetPropertyAttribute( std::string const& propertyName,
                                       std::string const& attributeName,
                                       boost::any value );

    /// Does the property with name propertyName have an attribute named
    /// attributeName?
    virtual bool HasPropertyAttribute( std::string const& propertyName,
                                       std::string const& attributeName ) const;

    /// Sets the enabled status of the property identified by propertyName.
    /// If the second argument is true, the property is enabled. If the second
    /// argument is false, the property is disabled.
    virtual void SetPropertyEnabled( std::string const& propertyName,
                                     bool enabled );

    /// Returns a list containing names of properties that have undergone a
    /// state change of some sort due to the most recent operation on a
    /// property in this set.
    virtual PSVectorOfStrings const& GetChanges();

    /// Clears the internal list of all property changes that have occurred
    /// recently. The maximum size of the internal list is equal to the number
    /// of properties in the set.
    virtual void ClearAccumulatedChanges();

    ///
    /// Toggles live properties if this set has any that are not live by default.
    /// Add MakeLive directives for any such properties inside an overridden
    /// version of this method. To enable autosaving of live properties, call
    /// PropertySet::EnableLiveProperties( live ) from inside the overridden
    /// version. Caution: Since PropertySet is an Object Relational Mapper,
    /// the *entire* propertyset, not just the changed property, is saved out
    /// every time an autosave occurs. This may be undesirable behavior for
    /// some propertysets. To ensure that autosaving does not occur, override
    /// this method in your derived class and do not call
    /// PropertySet::EnableLiveProperties() from within it.
    virtual void EnableLiveProperties( bool live );

    /// Calls Property::EmitValueChangedSignal for each Property in the set.
    /// One use of this is to ensure initial synchronization of application
    /// objects that rely on signals from live properties. Note, however, that
    /// this method will cause a change signal to be emitted from all properties
    /// in the set, not just the live ones.
    virtual void EmitValueChangedSignals() const;

    /// Sets the DataManager that should be used for Load(), LoadByKey(),
    /// Save(), Remove(), and SaveNoOverride()
    virtual void SetDataManager( crunchstore::DataManagerPtr manager );

    /// Loads values from PropertySet from the DEFAULT_ROLE store where uuid
    /// is equal to the uuid of this PropertySet
    virtual bool Load();

    /// Searches the DEFAULT_ROLE store for a PropertySet *of this type*
    /// having a property named KeyName with value KeyValue. If found, loads
    /// the values from the store into this PropertySet
    virtual bool LoadByKey( const std::string& KeyName, boost::any KeyValue );

    /// Saves this PropertySet to the DEFAULT_ROLE store
    virtual bool Save();

    /// Strictly writes out PropertySet to store; prevents derived-class overrides
    /// from performing other operations in the process.
    void SaveNoOverride();

    /// Deletes the current PropertySet from the DEFAULT_ROLE store
    virtual bool Remove();

    /// Returns true if the data in the set has changed since the last save
    /// operation.
    bool IsDirty(){ return m_writeDirty; }

protected:

    /// Slot that should be connected to any state-change signal emitted by any
    /// property in this set. This connection is taken care of in the 
    /// AddProperty method through a call to _connectChanges. Derived classes
    /// that override AddProperty should be sure to call _connectChanges if
    /// change accumulation is desired.
    /// ChangeAccumulator will accumulate a list of all properties
    /// that have undergone a change in value, attributes, or enabled state
    /// since the last call to GetChanges or the last call to
    /// ClearAccumulatedChanges
    virtual void ChangeAccumulator( PropertyPtr property );

    unsigned int GetBoostAnyVectorSize( const boost::any& value );

    ///
    /// Internal function that connects default signals to the change accumulator
    virtual void _connectChanges( PropertyPtr property );


    /// Callback method set up when argument to EnableLiveProperties is true.
    /// Checks whether a live property has changed and saves out the PropertySet
    /// if so.
    void SaveLiveProperties( Poco::Timer& timer );


    PSVectorOfStrings m_accumulatedChanges;

    ///The list of live objects for this property set
    std::vector< MakeLiveBasePtr > m_liveObjects;
    ///Determine whether the live properties should be live
    bool m_isLive;
private:
    // Empty string to use when need to return an empty string by reference
    std::string emptyString;
    // Empty PSVectorOfStrings to use when need to return an empty PSVOS by reference
    PSVectorOfStrings emptyPSVectorOfStrings;
    PSVectorOfStrings m_accumulatedChangesReturnable;

    crunchstore::DataManagerPtr m_dataManager;
    Poco::Timer* m_timer;
    bool m_writeDirty; ///< Data has changed since last write to db.
    bool m_liveWriteDirty; ///< Data in a live property has changed since last write to db

    Poco::Logger& m_logger;
    LogStreamPtr m_logStream;

};

} // namespace
