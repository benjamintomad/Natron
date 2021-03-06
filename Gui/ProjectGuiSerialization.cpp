//  Natron
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

#include "ProjectGuiSerialization.h"

#include "Global/Macros.h"

CLANG_DIAG_OFF(deprecated)
CLANG_DIAG_OFF(uninitialized)
#include <QtCore/QDebug>
#include <QSplitter>
#include <QVBoxLayout>
CLANG_DIAG_ON(deprecated)
CLANG_DIAG_ON(uninitialized)

#include "Engine/Project.h"
#include "Engine/Node.h"
#include "Engine/ParameterWrapper.h"
#include "Engine/ViewerInstance.h"


#include "Gui/NodeGui.h"
#include "Gui/Gui.h"
#include "Gui/TabWidget.h"
#include "Gui/ViewerTab.h"
#include "Gui/ViewerGL.h"
#include "Gui/ProjectGui.h"
#include "Gui/GuiApplicationManager.h"
#include "Gui/GuiAppInstance.h"
#include "Gui/NodeGraph.h"
#include "Gui/Histogram.h"
#include "Gui/Splitter.h"
#include "Gui/DockablePanel.h"
#include "Gui/ScriptEditor.h"
#include "Gui/PythonPanels.h"

void
ProjectGuiSerialization::initialize(const ProjectGui* projectGui)
{
    NodeList activeNodes;
    projectGui->getInternalProject()->getActiveNodesExpandGroups(&activeNodes);
    
    _serializedNodes.clear();
    for (NodeList::iterator it = activeNodes.begin(); it != activeNodes.end(); ++it) {
        boost::shared_ptr<NodeGuiI> nodegui_i = (*it)->getNodeGui();
        if (!nodegui_i) {
            continue;
        }
        boost::shared_ptr<NodeGui> nodegui = boost::dynamic_pointer_cast<NodeGui>(nodegui_i);
        
        if (nodegui->isVisible()) {
            NodeGuiSerialization state;
            nodegui->serialize(&state);
            _serializedNodes.push_back(state);
            ViewerInstance* viewer = dynamic_cast<ViewerInstance*>( (*it)->getLiveInstance() );
            if (viewer) {
                ViewerTab* tab = projectGui->getGui()->getViewerTabForInstance(viewer);
                assert(tab);
                ViewerData viewerData;
                double zoompar;
                tab->getViewer()->getProjection(&viewerData.zoomLeft, &viewerData.zoomBottom, &viewerData.zoomFactor, &zoompar);
                viewerData.userRoI = tab->getViewer()->getUserRegionOfInterest();
                viewerData.userRoIenabled = tab->getViewer()->isUserRegionOfInterestEnabled();
                viewerData.isClippedToProject = tab->isClippedToProject();
                viewerData.autoContrastEnabled = tab->isAutoContrastEnabled();
                viewerData.gain = tab->getGain();
                viewerData.colorSpace = tab->getColorSpace();
                viewerData.channels = tab->getChannelsString();
                viewerData.renderScaleActivated = tab->getRenderScaleActivated();
                viewerData.mipMapLevel = tab->getMipMapLevel();
                viewerData.zoomOrPanSinceLastFit = tab->getZoomOrPannedSinceLastFit();
                viewerData.wipeCompositingOp = (int)tab->getCompositingOperator();
                viewerData.leftToolbarVisible = tab->isLeftToolbarVisible();
                viewerData.rightToolbarVisible = tab->isRightToolbarVisible();
                viewerData.topToolbarVisible = tab->isTopToolbarVisible();
                viewerData.infobarVisible = tab->isInfobarVisible();
                viewerData.playerVisible = tab->isPlayerVisible();
                viewerData.timelineVisible = tab->isTimelineVisible();
                viewerData.checkerboardEnabled = tab->isCheckerboardEnabled();
                viewerData.fps = tab->getDesiredFps();
                viewerData.fpsLocked = tab->isFPSLocked();
                tab->getTimelineBounds(&viewerData.leftBound, &viewerData.rightBound);
                viewerData.version = VIEWER_DATA_SERIALIZATION_VERSION;
                _viewersData.insert( std::make_pair(viewer->getNode()->getScriptName_mt_safe(),viewerData) );
            }
        }
    }

    ///Init windows
    _layoutSerialization.initialize( projectGui->getGui() );

    ///save histograms
    std::list<Histogram*> histograms = projectGui->getGui()->getHistograms_mt_safe();
    for (std::list<Histogram*>::const_iterator it = histograms.begin(); it != histograms.end(); ++it) {
        _histograms.push_back( (*it)->objectName().toStdString() );
    }

    ///save opened panels by order
    QVBoxLayout* propLayout = projectGui->getGui()->getPropertiesLayout();
    for (int i = 0; i < propLayout->count(); ++i) {
        DockablePanel* isPanel = dynamic_cast<DockablePanel*>( propLayout->itemAt(i)->widget() );
        if ( isPanel && isPanel->isVisible() ) {
            KnobHolder* holder = isPanel->getHolder();
            assert(holder);
            
            Natron::EffectInstance* isEffect = dynamic_cast<Natron::EffectInstance*>(holder);
            Natron::Project* isProject = dynamic_cast<Natron::Project*>(holder);

            if (isProject) {
                _openedPanelsOrdered.push_back(kNatronProjectSettingsPanelSerializationName);
            } else if (isEffect) {
                _openedPanelsOrdered.push_back(isEffect->getNode()->getFullyQualifiedName());
            } 
        }
    }
    
    _scriptEditorInput = projectGui->getGui()->getScriptEditor()->getAutoSavedScript().toStdString();
    
    std::map<PyPanel*,std::string> pythonPanels = projectGui->getGui()->getPythonPanels();
    for ( std::map<PyPanel*,std::string>::iterator it = pythonPanels.begin(); it != pythonPanels.end(); ++it) {
        boost::shared_ptr<PythonPanelSerialization> s(new PythonPanelSerialization);
        s->initialize(it->first, it->second);
        _pythonPanels.push_back(s);
    }
} // initialize

void
PaneLayout::initialize(TabWidget* tab)
{
    QStringList children = tab->getTabScriptNames();

    for (int i = 0; i < children.size(); ++i) {
        tabs.push_back( children[i].toStdString() );
    }
    currentIndex = tab->activeIndex();
    name = tab->objectName_mt_safe().toStdString();
    isAnchor = tab->isAnchor();
}

void
SplitterSerialization::initialize(Splitter* splitter)
{
    sizes = splitter->serializeNatron().toStdString();
    Natron::OrientationEnum nO;
    Qt::Orientation qO = splitter->orientation();
    switch (qO) {
    case Qt::Horizontal:
        nO = Natron::eOrientationHorizontal;
        break;
    case Qt::Vertical:
        nO = Natron::eOrientationVertical;
        break;
    default:
        assert(false);
        break;
    }
    orientation = (int)nO;
    std::list<QWidget*> ch;
    splitter->getChildren_mt_safe(ch);
    assert(ch.size() == 2);

    for (std::list<QWidget*>::iterator it = ch.begin(); it != ch.end(); ++it) {
        Child *c = new Child;
        Splitter* isSplitter = dynamic_cast<Splitter*>(*it);
        TabWidget* isTabWidget = dynamic_cast<TabWidget*>(*it);
        if (isSplitter) {
            c->child_asSplitter = new SplitterSerialization;
            c->child_asSplitter->initialize(isSplitter);
        } else if (isTabWidget) {
            c->child_asPane = new PaneLayout;
            c->child_asPane->initialize(isTabWidget);
        }
        children.push_back(c);
    }
}

void
ApplicationWindowSerialization::initialize(bool mainWindow,
                                           SerializableWindow* widget)
{
    isMainWindow = mainWindow;
    widget->getMtSafePosition(x, y);
    widget->getMtSafeWindowSize(w, h);

    if (mainWindow) {
        Gui* gui = dynamic_cast<Gui*>(widget);
        assert(gui);
        QWidget* centralWidget = gui->getCentralWidget();
        Splitter* isSplitter = dynamic_cast<Splitter*>(centralWidget);
        TabWidget* isTabWidget = dynamic_cast<TabWidget*>(centralWidget);

        assert(isSplitter || isTabWidget);

        if (isSplitter) {
            child_asSplitter = new SplitterSerialization;
            child_asSplitter->initialize(isSplitter);
        } else {
            child_asPane = new PaneLayout;
            child_asPane->initialize(isTabWidget);
        }
    } else {
        FloatingWidget* isFloating = dynamic_cast<FloatingWidget*>(widget);
        assert(isFloating);

        QWidget* embedded = isFloating->getEmbeddedWidget();
        Splitter* isSplitter = dynamic_cast<Splitter*>(embedded);
        TabWidget* isTabWidget = dynamic_cast<TabWidget*>(embedded);
        DockablePanel* isPanel = dynamic_cast<DockablePanel*>(embedded);
        assert(isSplitter || isTabWidget || isPanel);

        if (isSplitter) {
            child_asSplitter = new SplitterSerialization;
            child_asSplitter->initialize(isSplitter);
        } else if (isTabWidget) {
            child_asPane = new PaneLayout;
            child_asPane->initialize(isTabWidget);
        } else {
            ///A named knob holder is a knob holder which has a unique name.
            NamedKnobHolder* isNamedHolder = dynamic_cast<NamedKnobHolder*>( isPanel->getHolder() );
            if (isNamedHolder) {
                child_asDockablePanel = isNamedHolder->getScriptName_mt_safe();
            } else {
                ///This must be the project settings panel
                child_asDockablePanel = kNatronProjectSettingsPanelSerializationName;
            }
        }
    }
} // initialize

void
GuiLayoutSerialization::initialize(Gui* gui)
{
    ApplicationWindowSerialization* mainWindow = new ApplicationWindowSerialization;

    mainWindow->initialize(true, gui);
    _windows.push_back(mainWindow);

    std::list<FloatingWidget*> floatingWindows = gui->getFloatingWindows();
    for (std::list<FloatingWidget*>::iterator it = floatingWindows.begin(); it != floatingWindows.end(); ++it) {
        ApplicationWindowSerialization* window = new ApplicationWindowSerialization;
        window->initialize(false, *it);
        _windows.push_back(window);
    }
}


void
PythonPanelSerialization::initialize(PyPanel* tab,const std::string& func)
{
    name = tab->getLabel();
    pythonFunction = func;
    std::list<Param*> parameters = tab->getParams();
    for (std::list<Param*>::iterator it = parameters.begin(); it != parameters.end(); ++it) {
        
        boost::shared_ptr<KnobI> knob = (*it)->getInternalKnob();
        Group_Knob* isGroup = dynamic_cast<Group_Knob*>( knob.get() );
        Page_Knob* isPage = dynamic_cast<Page_Knob*>( knob.get() );
        Button_Knob* isButton = dynamic_cast<Button_Knob*>( knob.get() );
        Choice_Knob* isChoice = dynamic_cast<Choice_Knob*>( knob.get() );
        
        if (!isGroup && !isPage && !isButton) {
            ///For choice do a deepclone because we need entries
            bool doCopyKnobs = isChoice != NULL;
            
            boost::shared_ptr<KnobSerialization> k(new KnobSerialization(knob,doCopyKnobs));
            knobs.push_back(k);
        }
        delete *it;
    }
    
    userData = tab->save_serialization_thread();
}

