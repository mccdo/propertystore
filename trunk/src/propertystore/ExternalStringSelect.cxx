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
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QFileDialog>
#include <QtGui/QFocusEvent>
#include <iostream>

Q_DECLARE_METATYPE(std::string)

namespace propertystore
{

ExternalStringSelect::ExternalStringSelect(QWidget *parent)
    : QWidget(parent)
{
    qRegisterMetaType<std::string>();
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    theLineEdit = new QLineEdit(this);
    theLineEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    QToolButton *button = new QToolButton(this);
    button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    button->setText(QLatin1String("Select..."));
    layout->addWidget(theLineEdit);
    layout->addWidget(button);
    setFocusProxy(theLineEdit);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_InputMethodEnabled);
    connect(theLineEdit, SIGNAL(textEdited(const QString &)),
                this, SIGNAL(stringChanged(const QString &)));
    connect(button, SIGNAL(clicked()),
                this, SLOT(buttonClicked()));
    connect( this, SIGNAL(ExternalStringSelectedQSignal(std::string)),
             this, SLOT(onExternalStringSelectedQueued(std::string)),
             Qt::QueuedConnection );
}

ExternalStringSelect* ExternalStringSelect::createNew( QWidget* parent )
{
    return new ExternalStringSelect( parent );
}

void ExternalStringSelect::buttonClicked()
{
    // Do nothing; derived classes should override to produce desired behavior,
    // such as opening a file dialog.
}

void ExternalStringSelect::onExternalStringSelected( const std::string& str )
{
    emit ExternalStringSelectedQSignal( str );
}

void ExternalStringSelect::onExternalStringSelectedQueued( const std::string str )
{
    QString text = text.fromStdString( str );
    theLineEdit->setText( text );
    emit stringChanged(text);
}

void ExternalStringSelect::focusInEvent(QFocusEvent *e)
{
    theLineEdit->event(e);
    if (e->reason() == Qt::TabFocusReason || e->reason() == Qt::BacktabFocusReason) {
        theLineEdit->selectAll();
    }
    QWidget::focusInEvent(e);
}

void ExternalStringSelect::focusOutEvent(QFocusEvent *e)
{
    theLineEdit->event(e);
    QWidget::focusOutEvent(e);
}

void ExternalStringSelect::keyPressEvent(QKeyEvent *e)
{
    theLineEdit->event(e);
}

void ExternalStringSelect::keyReleaseEvent(QKeyEvent *e)
{
    theLineEdit->event(e);
}

}
