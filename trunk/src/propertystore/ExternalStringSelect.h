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

/** This class is an altered version of the FileEdit class shown in
 Qt Quarterly at
http://doc.qt.nokia.com/qq/qq18-propertybrowser.html#extendingtheframework
It is being used in accordance with the terms of LGPL **/


#include <QtGui/QLineEdit>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <propertystore/Exports.h>

/// @file ExternalStringSelect.h
/// @namespace propertystore
/// @class ExternalStringSelect is a base implementation of an external string
/// selector widget consisting of a textedit field and an associated button.
/// In the base implementation, the button does nothing; the buttonClicked
/// method should be overridden in derived classes to provide the desired
/// behavior for externally selecting a string. Examples of this behavior are
/// launching a file chooser dialog or launching a color chooser dialog. The
/// execution path taken when the user clicks the button should eventually call
/// onExternalStringSelected() with the contents of the chosen string. Derived
/// classes must override buttonClicked and createNew.

namespace propertystore
{

class PROPERTYSTORE_EXPORT ExternalStringSelect : public QWidget
{
    Q_OBJECT
public:
    ExternalStringSelect( QWidget* parent = 0 );
    virtual void setString(const QString &str);
    QString string() const { return m_lineEdit->text(); }
    /// Sets attributes for the string associated with this instance of the editor.
    /// Derived classes can look at the values stored in m_attributes to
    /// tweak display or input of the string.
    void SetStringAttributes( const QMap<std::string, std::string>& attributes );

    /// Returns a pointer to a new one of these. Used by ExternalStringSelectFactory
    /// to create new versions of this object. This allows the developer to derive
    /// from this class, and pass a pointer to the derived class to
    /// ExternalStringSelectFactory::setEditorType. The factory can then create
    /// instances of the derived class as needed.
    virtual ExternalStringSelect* createNew( QWidget* parent );
Q_SIGNALS:
    void stringChanged(const QString &str);
    void ExternalStringSelectedQSignal( const std::string str );

protected:
    void focusInEvent( QFocusEvent* e );
    void focusOutEvent( QFocusEvent* e );
    void keyPressEvent( QKeyEvent* e );
    void keyReleaseEvent( QKeyEvent* e );

public Q_SLOTS:
    virtual void buttonClicked();
    virtual void onExternalStringSelected( const std::string& str );
    virtual void onExternalStringSelectedQueued( const std::string str );

protected:
    QLineEdit* m_lineEdit;
    QHBoxLayout* m_layout;
    QToolButton* m_button;
    /// Derived classes can look through this map for attributes that control
    /// specialized string input or display
    QMap< std::string, std::string > m_attributes;
};

}

