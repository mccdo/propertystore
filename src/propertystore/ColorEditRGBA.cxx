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
#include <propertystore/ColorEditRGBA.h>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QFileDialog>
#include <QtGui/QFocusEvent>
#include <qtpropertybrowserutils_p.h>

namespace propertystore
{

ColorEditRGBA::ColorEditRGBA(QWidget *parent)
    : ExternalStringSelect( parent ),
      m_colorDialog( 0 )
{
    // Insert color swatch into the property display. We don't yet have a color,
    // so wait until setString to color the swatch.
    m_colorSwatch = new QLabel( this );
    m_layout->insertWidget( 0, m_colorSwatch, 0 );
}
////////////////////////////////////////////////////////////////////////////////
ExternalStringSelect* ColorEditRGBA::createNew( QWidget* parent )
{
    return new ColorEditRGBA( parent );
}
////////////////////////////////////////////////////////////////////////////////
void ColorEditRGBA::setString(const QString &str)
{
    ExternalStringSelect::setString( str );
    m_colorSwatch->setPixmap( QtPropertyBrowserUtils::brushValuePixmap(
                    QBrush( ConvertStringToQColor() ) ) );
}
////////////////////////////////////////////////////////////////////////////////
QColor ColorEditRGBA::ConvertStringToQColor()
{
    QStringList colors = string().split( "," );
    int r, g, b, a;
    r = g = b = a = 255;
    if( colors.size() == 4 )
    {
        r = colors.at(0).toInt();
        g = colors.at(1).toInt();
        b = colors.at(2).toInt();
        a = colors.at(3).toInt();
    }
    if( (r > -1) && (r < 256) && (g > -1) && (g < 256) &&
            (b > -1) && (b < 256) && (a > -1) && (a < 256) )
    {
        return QColor( r, g, b, a );
    }
    else
    {
        return QColor( 255, 255, 255, 255 );
    }
}
////////////////////////////////////////////////////////////////////////////////
void ColorEditRGBA::buttonClicked()
{
    if( m_colorDialog )
    {
        m_colorDialog->raise();
        return;
    }

    m_colorDialog = new QColorDialog( ConvertStringToQColor(), 0 );
    m_colorDialog->setOptions( QColorDialog::ShowAlphaChannel );
    m_colorDialog->setAttribute( Qt::WA_DeleteOnClose );

    connect( m_colorDialog, SIGNAL( colorSelected(QColor) ),
                      this, SLOT( onColorSelected(QColor) ) );
    connect( m_colorDialog, SIGNAL( rejected() ), this,
                      SLOT( onColorCancelled() ) );

    m_colorDialog->show();
}
////////////////////////////////////////////////////////////////////////////////
void ColorEditRGBA::onColorSelected( const QColor& color )
{
    QString red, green, blue, alpha;
    red.setNum( color.red() );
    green.setNum( color.green() );
    blue.setNum( color.blue() );
    alpha.setNum( color.alpha() );

    QString colorText = red + "," + green + "," + blue + "," + alpha;

    // Now that we've pre-processed the color, let the base class handle updating
    // everything
    onExternalStringSelected( colorText.toStdString() );

    m_colorSwatch->setPixmap(
                QtPropertyBrowserUtils::brushValuePixmap( QBrush( color ) ) );

    if ( m_colorDialog != 0 )
    {
        m_colorDialog->close();
        m_colorDialog = 0;
    }
}
////////////////////////////////////////////////////////////////////////////////
void ColorEditRGBA::onColorCancelled()
{
    if ( m_colorDialog != 0 )
    {
        m_colorDialog->close();
        m_colorDialog = 0;
    }
}
////////////////////////////////////////////////////////////////////////////////
}
