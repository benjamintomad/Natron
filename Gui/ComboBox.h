//  Natron
//
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
*Created by Alexandre GAUTHIER-FOICHAT on 6/1/2012. 
*contact: immarespond at gmail dot com
*
*/

#ifndef POWITER_GUI_COMBOBOX_H_
#define POWITER_GUI_COMBOBOX_H_

#include <vector>
#include <QFrame>
#include <QtGui/QKeySequence>
#include <QtGui/QIcon>
#include <QMenu>

class QHBoxLayout;
class QLabel;
class QMouseEvent;
class QAction;

class MenuWithToolTips : public QMenu{
    Q_OBJECT
public:
    MenuWithToolTips(QWidget* parent):QMenu(parent){}
    
    bool event (QEvent * e)
    {
        const QHelpEvent *helpEvent = static_cast <QHelpEvent *>(e);
        if (helpEvent->type() == QEvent::ToolTip) {
            QToolTip::showText(helpEvent->globalPos(), activeAction()->toolTip());
        } else {
            QToolTip::hideText();
        }
        return QMenu::event(e);
    }
};


class ComboBox : public QFrame
{
 private:
    Q_OBJECT
    
    Q_PROPERTY( bool pressed READ isPressed WRITE setPressed)
    
    QHBoxLayout* _mainLayout;
    QLabel* _currentText;
    QLabel* _dropDownIcon;
    
    int _currentIndex;
    
    int _maximumTextSize;
    
    bool pressed;
    
    std::vector<int> _separators;
    std::vector<QAction*> _actions;
    MenuWithToolTips* _menu;

public:
    
    explicit ComboBox(QWidget* parent = 0);
    
    virtual ~ComboBox(){}
    
    /*Insert a new item BEFORE the specified index.*/
    void insertItem(int index,const QString& item,QIcon icon = QIcon(),QKeySequence = QKeySequence(),const QString& toolTip = QString());
    
    void addItem(const QString& item,QIcon icon = QIcon(),QKeySequence = QKeySequence(),const QString& toolTip = QString());
        
    /*Appends a separator to the comboBox.*/
    void addSeparator();
    
    /*Insert a separator BEFORE the item at the index specified.*/
    void insertSeparator(int index);
    
    QString itemText(int index) const;
    
    int count() const;
    
    int itemIndex(const QString& str) const;
    
    void removeItem(const QString& item);
    
    void disableItem(int index);
    
    void enableItem(int index);
    
    void setItemText(int index,const QString& item);
    
    void setItemShortcut(int index,const QKeySequence& sequence);
    
    void setItemIcon(int index,const QIcon& icon);
    
    bool isPressed() const {return pressed;}
    
    void setPressed(bool b) {pressed = b;}
    
    void setCurrentText(const QString& text);
    
    /*this function returns the displayed text with some padding
     ,i.e two spaces before and after the text, as such:
     "  test  " .*/
    QString text() const;
    
    int activeIndex() const;
    
    void clear();
    
    public slots:
    
    void setCurrentIndex(int index);
    
    
signals:
    void currentIndexChanged(int index);
    void currentIndexChanged(const QString &text);
    
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    
private:
    void adjustSize(const QString& str);
    void createMenu();
    
};

#endif
