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

#ifndef OVERLAYSUPPORT_H
#define OVERLAYSUPPORT_H

// from <https://docs.python.org/3/c-api/intro.html#include-files>:
// "Since Python may define some pre-processor definitions which affect the standard headers on some systems, you must include Python.h before any standard headers are included."
#include <Python.h>

/**
 * @class An abstract interface for overlay holders. Any OpenGL widget capable of drawing overlays should
 * implement this interface.
 **/

class OverlaySupport
{
public:

    OverlaySupport()
    {
    }

    virtual ~OverlaySupport()
    {
    }

    /**
     * @brief Swap the OpenGL buffers.
     **/
    virtual void swapOpenGLBuffers() = 0;

    /**
     * @brief Repaint
     **/
    virtual void redraw() = 0;

    /**
     * @brief Returns the width and height of the viewport in window coordinates.
     **/
    virtual void getViewportSize(double &width, double &height) const = 0;

    /**
     * @brief Returns the pixel scale of the viewport.
     **/
    virtual void getPixelScale(double & xScale, double & yScale) const  = 0;

    /**
     * @brief Returns the colour of the background (i.e: clear color) of the viewport.
     **/
    virtual void getBackgroundColour(double &r, double &g, double &b) const = 0;
    
    /**
     * @brief Must save all relevant OpenGL bits so that they can be restored as-is after the draw action of a plugin.
     **/
    virtual void saveOpenGLContext() = 0;
    
    /**
     * @brief Must restore all OpenGL bits saved in saveOpenGLContext()
     **/
    virtual void restoreOpenGLContext() = 0;
    
    /**
     * @brief Get the current mipmapLevel applied by the viewer
     **/
    virtual unsigned int getCurrentRenderScale() const = 0;
};

#endif // OVERLAYSUPPORT_H
