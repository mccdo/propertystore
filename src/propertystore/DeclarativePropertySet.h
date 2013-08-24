#pragma once

#include <propertystore/PropertySet.h>
#include <propertystore/MakeLivePtr.h>
#include <propertystore/MakeLive.h>
#include <propertystore/Exports.h>

#include <vector>

#include <Poco/JSON/Object.h>

namespace propertystore
{

struct delayDetails
{
    delayDetails( const std::string& name, const std::string& sigName, bool sendID ):
        propertyName( name ),
        signalName( sigName ),
        sendUUID( sendID )
    {

    }

    std::string propertyName;
    std::string signalName;
    bool sendUUID;
};

class PROPERTYSTORE_EXPORT DeclarativePropertySet : public PropertySet
{
public:
    DeclarativePropertySet();

    virtual void EnableLiveProperties( bool enable );

    void FromJSON( const std::string& json );

    void FromJSONFile( const std::string& filePath );

private:
    std::string GetUILabel( Poco::JSON::Object::Ptr property );
    void SetMinMax( const std::string& name, Poco::JSON::Object::Ptr property );
    void AddInteger( const std::string& name, Poco::JSON::Object::Ptr property );
    void AddDouble( const std::string& name, Poco::JSON::Object::Ptr property );
    void AddFloat( const std::string& name, Poco::JSON::Object::Ptr property );
    void AddBoolean( const std::string& name, Poco::JSON::Object::Ptr property );
    void AddString( const std::string& name, Poco::JSON::Object::Ptr property );
    void AddBLOB( const std::string& name, Poco::JSON::Object::Ptr property );

    template <typename T>
    void ProcessLiveSignal( const std::string& name,
                            Poco::JSON::Object::Ptr property,
                            std::vector< delayDetails >& storageVector )
    {
        if( property->has( "liveSignalName" ) )
        {
            std::string sigName = property->getValue< std::string >( "liveSignalName" );
            bool sendUUID = property->optValue< bool >( "liveSendUUID", true );
            std::string liveType = property->optValue< std::string >( "live", "immediate" );

            if( "delayed" == liveType )
            {
                delayDetails details( name, sigName, sendUUID );
                storageVector.push_back( details );
            }
            else // "immediate" == livetype
            {
                MakeLiveBasePtr p(
                     new MakeLive< const T& >( this->GetUUIDAsString(),
                         this->GetProperty( name ),
                         sigName, sendUUID ) );

                m_liveObjects.push_back( p );
            }

            property->remove( "liveSignalName" );
            property->remove( "liveSignalUUID" );
            property->remove( "live" );
        }
    }


    template <typename T>
    void EnableDelayedLiveSignals( std::vector< delayDetails >& storageVector )
    {
        std::vector< delayDetails >::iterator itr;

        itr = storageVector.begin();
        while( itr != storageVector.end() )
        {
            MakeLiveBasePtr p(
                 new MakeLive< const T& >( this->GetUUIDAsString(),
                     this->GetProperty( itr->propertyName ),
                     itr->signalName, itr->sendUUID ) );

            m_liveObjects.push_back( p );
            ++itr;
        }
    }

    std::vector< delayDetails > m_delayedIntegers;
    std::vector< delayDetails > m_delayedFloats;
    std::vector< delayDetails > m_delayedDoubles;
    std::vector< delayDetails > m_delayedBooleans;
    std::vector< delayDetails > m_delayedStrings;
    std::vector< delayDetails > m_delayedBLOBs;
};

typedef propertystore::ClassPtrDef<DeclarativePropertySet>::type  DeclarativePropertySetPtr;
} // namespace
