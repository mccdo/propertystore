#include <propertystore/UnitConverter.h>

#include <cmath>
#include <stdexcept>

namespace propertystore
{
////////////////////////////////////////////////////////////////////////////////
conversion::conversion()
{

}
////////////////////////////////////////////////////////////////////////////////
conversion::conversion( const std::string& from_unit,
                        const std::string& to_unit,
                        double Coeff,
                        double Offset,
                        double Exp1,
                        double Exp2 ):
        m_from( from_unit ),
        m_to( to_unit ),
        m_coeff( Coeff ),
        m_offset( Offset ),
        m_exp1( Exp1 ),
        m_exp2( Exp2 )
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
conversion::~conversion()
{
    ;
}
////////////////////////////////////////////////////////////////////////////////
double conversion::operator ()( double value )
{
    return std::pow( m_offset + m_coeff*std::pow( value, m_exp1 ) , m_exp2 );
}
////////////////////////////////////////////////////////////////////////////////
UnitConverter::UnitConverter()
{
    SetupDefaultConversions();
}
////////////////////////////////////////////////////////////////////////////////
void UnitConverter::AddConversion( const std::string &from_unit,
                                   const std::string &to_unit,
                                   double Coeff,
                                   double Offset,
                                   double Exp1,
                                   double Exp2 )
{
    conversionPtr conv( new conversion( from_unit, to_unit, Coeff, Offset, Exp1, Exp2 ) );
    std::string key( from_unit );
    key += to_unit;
    m_conversions[ key ] = conv;
}
////////////////////////////////////////////////////////////////////////////////
void UnitConverter::AddConversionInversePair( const std::string& from_unit,
                               const std::string& to_unit,
                               double Coeff )
{
    AddConversion( from_unit, to_unit, Coeff );
    AddConversion( to_unit, from_unit, 1.0/Coeff );
}
////////////////////////////////////////////////////////////////////////////////
double UnitConverter::Convert( double value,
                               const std::string &from_unit,
                               const std::string &to_unit ) const
{
    std::string key( from_unit );
    key += to_unit;
    std::map< std::string, conversionPtr >::const_iterator itr =
            m_conversions.find( key );
    if( itr == m_conversions.end() )
    {
        std::string message;
        message = "No conversion available from " + from_unit + " to " + to_unit + ".";
        throw std::runtime_error( message );
    }

    return itr->second->operator()( value );
}
////////////////////////////////////////////////////////////////////////////////
void UnitConverter::SetupDefaultConversions()
{
    // Length
        AddConversionInversePair( "meter", "yard", 1.0936133 );
        AddConversionInversePair( "meter", "foot", 3.2808399 );
        AddConversionInversePair( "meter", "inch", 39.370079 );
        AddConversionInversePair( "meter", "micrometer", 1000000.0 );
        AddConversionInversePair( "meter", "millimeter", 1000.0 );
        AddConversionInversePair( "meter", "centimeter", 100.0 );
        AddConversionInversePair( "meter", "kilometer", 1.0/1000.0 );

        AddConversionInversePair( "mil", "micrometer", 25.4 );

        // Volume
        AddConversionInversePair( "gallon", "liter", 3.78541 );
        AddConversionInversePair( "fluidounce", "milliliter", 29.5735 );
        AddConversionInversePair( "fluidounce", "liter", 0.0295735 );

        // Pressure
        AddConversionInversePair( "psi", "megapascal", 0.00689475729 );
        AddConversionInversePair( "psi", "kilopascal", 6.89475729 );
        AddConversionInversePair( "psi", "bar", 0.0689475729 );


        // With some more clever parsing of the available conversions I should
        // be able to eliminate stuff like this by doing two-stage conversions.
        // First you make a table of the available conversions that go from the
        // from_unit to anything, then you make a table of the available conversions
        // that go from anything to the to_unit. Then you see if there's any overlap
        // in the two "anything" columns. If so, you have a conversion path.
        AddConversionInversePair( "centimeter", "millimeter", 10.0 );
        AddConversionInversePair( "inch", "centimeter", 2.54 );
        AddConversionInversePair( "inch", "millimeter", 25.4 );
}
////////////////////////////////////////////////////////////////////////////////

} // namespace
