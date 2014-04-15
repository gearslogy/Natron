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

#ifndef NATRON_GUI_VIEWERTAB_H_
#define NATRON_GUI_VIEWERTAB_H_ 

#include "Global/Macros.h"
CLANG_DIAG_OFF(deprecated)
#include <QWidget>
CLANG_DIAG_ON(deprecated)
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "Global/GlobalDefines.h"

class ViewerGL;
class ViewerInstance;
class Gui;
class RectI;
class Format;

struct ViewerTabPrivate;
class ViewerTab: public QWidget 
{ 
    Q_OBJECT
    
public:
    explicit ViewerTab(Gui* gui,ViewerInstance* node,QWidget* parent=0);
    
	virtual ~ViewerTab() OVERRIDE;
    
    
    ViewerInstance* getInternalNode() const;

    Gui* getGui() const;
    
    ViewerGL* getViewer() const;
    
    void setCurrentView(int view);
    
    int getCurrentView() const;
    
    void seek(SequenceTime time);
    
    /**
     *@brief Tells all the nodes in the grpah to draw their overlays
     **/
    /*All the overlay methods are forwarding calls to the default node instance*/
    void drawOverlays(double scaleX,double scaleY) const;
    
    bool notifyOverlaysPenDown(double scaleX,double scaleY,const QPointF& viewportPos,const QPointF& pos);
    
    bool notifyOverlaysPenMotion(double scaleX,double scaleY,const QPointF& viewportPos,const QPointF& pos);
    
    bool notifyOverlaysPenUp(double scaleX,double scaleY,const QPointF& viewportPos,const QPointF& pos);
    
    bool notifyOverlaysKeyDown(double scaleX,double scaleY,QKeyEvent* e);
    
    bool notifyOverlaysKeyUp(double scaleX,double scaleY,QKeyEvent* e);
    
    bool notifyOverlaysKeyRepeat(double scaleX,double scaleY,QKeyEvent* e);
    
    bool notifyOverlaysFocusGained(double scaleX,double scaleY);
    
    bool notifyOverlaysFocusLost(double scaleX,double scaleY);
    
    
    
    ////////
    /////////////The following functions are used when serializing/deserializing the project gui
    ///////////// so the viewer can restore the exact same settings to the user.
    bool isClippedToProject() const;
    
    std::string getColorSpace() const;
    
    void setUserRoIEnabled(bool b);
    
    void setUserRoI(const RectI& r);
    
    void setClipToProject(bool b);
    
    void setColorSpace(const std::string& colorSpaceName);
    
    void setGain(double d);
    
    double getGain() const;
    
    std::string getChannelsString() const;
    
    void setChannels(const std::string& channelsStr);
    
    bool isAutoContrastEnabled() const;
    
    void setAutoContrastEnabled(bool b);
    
    void setMipMapLevel(int level);
    
    int getMipMapLevel() const;
    
    void setRenderScaleActivated(bool act);
    
    bool getRenderScaleActivated() const;
    
    void setInfoBarResolution(const Format& f);
    
public slots:
    
    void startPause(bool);
    void abortRendering();
    void startBackward(bool);
    void previousFrame();
    void nextFrame();
    void previousIncrement();
    void nextIncrement();
    void firstFrame();
    void lastFrame();
    void centerViewer();
    void toggleLoopMode(bool);
    void onViewerChannelsChanged(int);
    void onClipToProjectButtonToggle(bool);
    void onTimeLineTimeChanged(SequenceTime time,int);
    void onCurrentTimeSpinBoxChanged(double);
    /*Updates the comboBox according to the real zoomFactor. Value is in % */
    void updateZoomComboBox(int value);
    
    void onRenderScaleComboIndexChanged(int index);
    
    /*makes the viewer black*/
    void disconnectViewer();
    
    
    void onEngineStarted(bool forward,int frameCount);
    
    void onEngineStopped();
    
    void refresh();
    
    void updateViewsMenu(int count);
    
    void showView(int view);
    
    void onEnableViewerRoIButtonToggle(bool);
        
    void onAutoContrastChanged(bool b);

    void onRenderScaleButtonClicked(bool checked);

private:
        
    bool eventFilter(QObject *target, QEvent *event);
    
    virtual void keyPressEvent(QKeyEvent* e) OVERRIDE FINAL;
    
    virtual void enterEvent(QEvent*) OVERRIDE FINAL;

    virtual QSize minimumSizeHint() const OVERRIDE FINAL;
    
    virtual QSize sizeHint() const OVERRIDE FINAL;

    
    boost::scoped_ptr<ViewerTabPrivate> _imp;
};

#endif // NATRON_GUI_VIEWERTAB_H_
