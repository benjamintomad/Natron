//  Natron
//
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/*
 * Created by Alexandre GAUTHIER-FOICHAT on 6/1/2012.
 * contact: immarespond at gmail dot com
 *
 */

// from <https://docs.python.org/3/c-api/intro.html#include-files>:
// "Since Python may define some pre-processor definitions which affect the standard headers on some systems, you must include Python.h before any standard headers are included."
#include <Python.h>

#include "ToolButton.h"

CLANG_DIAG_OFF(deprecated)
#include <QMenu>
CLANG_DIAG_ON(deprecated)

#include <boost/weak_ptr.hpp>
#include "Gui/GuiAppInstance.h"
#include "Gui/Gui.h"
#include "Engine/Project.h"

struct ToolButtonPrivate
{
    GuiAppInstance* _app;
    QString _id;
    int _major,_minor;
    QString _label;
    QIcon _icon;
    QMenu* _menu;
    std::vector<ToolButton*> _children;
    QAction* _action;
    boost::weak_ptr<PluginGroupNode> _pluginToolButton;

    ToolButtonPrivate(GuiAppInstance* app,
                      const boost::shared_ptr<PluginGroupNode>& pluginToolButton,
                      const QString & pluginID,
                      int major,
                      int minor,
                      const QString & label,
                      QIcon icon)
        : _app(app)
          , _id(pluginID)
          , _major(major)
          , _minor(minor)
          , _label(label)
          , _icon(icon)
          , _menu(NULL)
          , _children()
          , _action(NULL)
          , _pluginToolButton(pluginToolButton)
    {
    }
};


ToolButton::ToolButton(GuiAppInstance* app,
                       const boost::shared_ptr<PluginGroupNode>& pluginToolButton,
                       const QString & pluginID,
                       int major,
                       int minor,
                       const QString & label,
                       QIcon icon)
    : _imp( new ToolButtonPrivate(app,pluginToolButton,pluginID,major,minor,label,icon) )
{
}

ToolButton::~ToolButton()
{
}

const QString &
ToolButton::getID() const
{
    return _imp->_id;
}

int
ToolButton::getPluginMajor() const
{
    return _imp->_major;
}

int
ToolButton::getPluginMinor() const
{
    return _imp->_minor;
}

const QString &
ToolButton::getLabel() const
{
    return _imp->_label;
};
const QIcon &
ToolButton::getIcon() const
{
    return _imp->_icon;
};
bool
ToolButton::hasChildren() const
{
    return !( _imp->_children.empty() );
}

QMenu*
ToolButton::getMenu() const
{
    assert(_imp->_menu); return _imp->_menu;
}

void
ToolButton::setMenu(QMenu* menu )
{
    _imp->_menu = menu;
}

void
ToolButton::tryAddChild(ToolButton* child)
{
    assert(_imp->_menu);
    for (unsigned int i = 0; i < _imp->_children.size(); ++i) {
        if (_imp->_children[i] == child) {
            return;
        }
    }
    _imp->_children.push_back(child);
    _imp->_menu->addAction( child->getAction() );
}

const std::vector<ToolButton*> &
ToolButton::getChildren() const
{
    return _imp->_children;
}

QAction*
ToolButton::getAction() const
{
    return _imp->_action;
}

void
ToolButton::setAction(QAction* action)
{
    _imp->_action = action;
}

boost::shared_ptr<PluginGroupNode>
ToolButton::getPluginToolButton() const
{
    return _imp->_pluginToolButton.lock();
}

void
ToolButton::onTriggered()
{
    boost::shared_ptr<NodeCollection> group = _imp->_app->getGui()->getLastSelectedNodeCollection();
    assert(group);
    CreateNodeArgs args(_imp->_id,
                        "",
                        _imp->_major,_imp->_minor,
                        true,
                        INT_MIN,INT_MIN,
                        true,
                        true,
                        QString(),
                        CreateNodeArgs::DefaultValuesList(),
                        group);
    _imp->_app->createNode( args );
}

