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
#include <propertystore/ExternalStringSelectManager.h>
#include <qtpropertybrowserutils_p.h>

namespace propertystore
{
////////////////////////////////////////////////////////////////////////////////
QString ExternalStringSelectManager::value(const QtProperty* property) const
{
    if (!theValues.contains(property))
        return QString();
    return theValues[property].value;
}
////////////////////////////////////////////////////////////////////////////////
QIcon ExternalStringSelectManager::valueIcon(const QtProperty* property) const
{
    QString text = theValues[property].value;

    // This is a special case to support an icon for a color string without
    // having to provide a custom manager for colors. We assume a string is a
    // color if it has exactly four comma-separated substrings that can be
    // converted to integers in the range [0,255]. That criterion should be
    // specific enough to avoid providing a color icon for things that aren't
    // supposed to be colors.
    QStringList colors = text.split( "," );
    if( colors.size() != 4 )
    {
        return QIcon();
    }

    int r, g, b, a;
    r = g = b = a = -1;
    r = colors.at( 0 ).toInt();
    g = colors.at( 1 ).toInt();
    b = colors.at( 2 ).toInt();
    a = colors.at( 3 ).toInt();
    if( (r > -1) && (r < 256) && (g > -1) && (g < 256) &&
            (b > -1) && (b < 256) && (a > -1) && (a < 256) )
    {
        return QtPropertyBrowserUtils::brushValueIcon(QBrush(
                                                        QColor( r, g, b, a ) ));
    }

    return QIcon();
}
////////////////////////////////////////////////////////////////////////////////
void ExternalStringSelectManager::setValue(QtProperty* property, const QString& val)
{
    if (!theValues.contains(property))
        return;

    Data data = theValues[property];

    if (data.value == val)
        return;

    data.value = val;

    theValues[property] = data;

    emit propertyChanged(property);
    emit valueChanged(property, data.value);
}
////////////////////////////////////////////////////////////////////////////////
}

