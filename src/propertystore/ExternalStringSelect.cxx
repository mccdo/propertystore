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
#include <propertystore/ExternalStringSelect.h>
#include <QtGui/QFocusEvent>

Q_DECLARE_METATYPE(std::string)

namespace propertystore
{

ExternalStringSelect::ExternalStringSelect(QWidget* parent)
    : QWidget(parent)
{
    qRegisterMetaType<std::string>();
    m_layout = new QHBoxLayout( this );
    m_layout->setMargin( 0 );
    m_layout->setSpacing( 0 );
    m_lineEdit = new QLineEdit( this );
    m_lineEdit->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred) );
    m_button = new QToolButton( this );
    m_button->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    m_button->setText( QLatin1String("Select...") );
    m_layout->addWidget( m_lineEdit );
    m_layout->addWidget( m_button );
    setFocusProxy( m_lineEdit );
    setFocusPolicy( Qt::StrongFocus );
    setAttribute( Qt::WA_InputMethodEnabled );
    connect( m_lineEdit, SIGNAL(textEdited(const QString &)),
                this, SIGNAL(stringChanged(const QString &)) );
    connect( m_button, SIGNAL(clicked()),
                this, SLOT(buttonClicked()) );
    connect( this, SIGNAL(ExternalStringSelectedQSignal(std::string)),
             this, SLOT(onExternalStringSelectedQueued(std::string)),
             Qt::QueuedConnection );
}
////////////////////////////////////////////////////////////////////////////////
ExternalStringSelect* ExternalStringSelect::createNew( QWidget* parent )
{
    return new ExternalStringSelect( parent );
}
////////////////////////////////////////////////////////////////////////////////
void ExternalStringSelect::setString(const QString &str)
{
    if (m_lineEdit->text() != str)
    {
        m_lineEdit->setText(str);
    }
}
////////////////////////////////////////////////////////////////////////////////
void ExternalStringSelect::buttonClicked()
{
    // Do nothing; derived classes should override to produce desired behavior,
    // such as opening a file dialog.
}
////////////////////////////////////////////////////////////////////////////////
void ExternalStringSelect::onExternalStringSelected( const std::string& str )
{
    emit ExternalStringSelectedQSignal( str );
}
////////////////////////////////////////////////////////////////////////////////
void ExternalStringSelect::onExternalStringSelectedQueued( const std::string str )
{
    QString text = text.fromStdString( str );
    m_lineEdit->setText( text );
    emit stringChanged(text);
}
////////////////////////////////////////////////////////////////////////////////
void ExternalStringSelect::focusInEvent(QFocusEvent *e)
{
    m_lineEdit->event(e);
    if (e->reason() == Qt::TabFocusReason || e->reason() == Qt::BacktabFocusReason) {
        m_lineEdit->selectAll();
    }
    QWidget::focusInEvent(e);
}
////////////////////////////////////////////////////////////////////////////////
void ExternalStringSelect::focusOutEvent(QFocusEvent *e)
{
    m_lineEdit->event(e);
    QWidget::focusOutEvent(e);
}
////////////////////////////////////////////////////////////////////////////////
void ExternalStringSelect::keyPressEvent(QKeyEvent *e)
{
    m_lineEdit->event(e);
}
////////////////////////////////////////////////////////////////////////////////
void ExternalStringSelect::keyReleaseEvent(QKeyEvent *e)
{
    m_lineEdit->event(e);
}
////////////////////////////////////////////////////////////////////////////////
}
