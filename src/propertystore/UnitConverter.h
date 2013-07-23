#pragma once

#include <string>
#include <map>
#include <propertystore/PointerTypes.h>

namespace propertystore
{
/// The possible unit *systems* we know about
enum DisplayUnitSystem { SI_Units, US_Units };

// First declare a simple class we will use to store conversion information
class conversion
{
public:
    conversion();
    conversion( const std::string& from_unit,
                const std::string& to_unit,
                double Coeff,
                double Offset = 0,
                double Exp1 = 1,
                double Exp2 = 1  );
    virtual ~conversion();
    virtual double operator()( double value );

private:
    std::string m_from;
    std::string m_to;
    double m_coeff;
    double m_offset;
    double m_exp1;
    double m_exp2;
};

typedef propertystore::ClassPtrDef<conversion>::type  conversionPtr;

// And now the more important developer-facing class:
/// Holds a set of conversion objects capable of converting from one unit to
/// another, and provides a single access point for calling these conversion
/// objects.
class UnitConverter
{
public:
    UnitConverter();

    /// Add a conversion from from_unit to to_unit of the form
    /// to_unit = (Offset + Coeff*from_unit^Exp1)^Exp2
    /// This form allows us to handle the usual linear transformations involved
    /// in unit conversions as well as a large, but not exhaustive, set of the
    /// other affine transformations (such as temperature).
    void AddConversion( const std::string& from_unit, const std::string& to_unit, double Coeff, double Offset = 0, double Exp1 = 1, double Exp2 = 1 );

    /// This is a convenience function for adding a pair of conversions that
    /// are simple multiplicative inverses of each other. For example,
    /// meter->yard and yard->meter are simple multiplicative inverses, whereas
    /// fahrenheit->celsius and celsius->fahrenheit are not.
    void AddConversionInversePair( const std::string& from_unit, const std::string& to_unit, double Coeff );

    /// Converts value to to_unit from from_unit
    double Convert( double value, const std::string& from_unit, const std::string& to_unit ) const;

private:
    void SetupDefaultConversions();
    std::map< std::string, conversionPtr > m_conversions;
};

} // namespace
