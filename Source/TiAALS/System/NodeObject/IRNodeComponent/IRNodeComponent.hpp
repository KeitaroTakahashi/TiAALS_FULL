//
//  NodeComponent.hpp
//  NodeComponentObject_Study - App
//
//  Created by Keitaro on 20/08/2018.
//

#ifndef NodeComponent_hpp
#define NodeComponent_hpp


#include "JuceHeader.h"
#include "json11.hpp"
#include "IRFoundation.h"
#include "IRStrComponent.hpp"
#include "IRSaveLoadSystem.hpp"
#include "IRNodeObjectStatusStruct.h"
//#include "IRLinkFoundation.hpp"
#include "Benchmark.h"
#include "IRResizeSquare2.h"

#include "IRHeavyWeightComponent.h"

enum IRNodeComponentSetUp
{
    // resizing
    IREnableResizableFlag,
    IREnableWidthResizingFlag,
    IREnableHeightResizingFlag,
    // selecting
    IREnableSelectableFlag,
    
    //drag
    IREnableMovableFlag,
    
    //copy
    IREnableCopiableFlag,
    
    //delete
    IREnableDeletableFlag,
    
    IRNodeComponentSetUpUndefined
};


enum IRNodeComponentStatus
{
    EditModeStatus, // global status for workspace
    LinkModeStatus,

    LinkedStatus,
    
    SelectableStatus,
    CopiableStatus,
    DeletableStatus,
    MovableStatus,

    ResizableStatus,
    HeightResizableStatus,
    WidthResizableStatus,
    WillResizeStatus,
    HasResizedStatus,
    
    IRNodeComponentStatusUndefined
};


// heavy light weight component
// lightWeightComponent is set as a default
enum IRNodeComponentType
{
    // juce oriented component
    lightWeightComponent,
    // os oriented object such as VideoComponent, embbed component, OpenGL etc.
    heavyWeightComponent,
    
    // this is basically the heavyWeightComponent but does not contain any heavy Weight component on it for instance, IRVideoPlayerObject and IRSpectrogram have heavyWeightComponents on themselves so use heavyWeightComponent instead of ordinaryIRComponent
    ordinaryIRComponent
};

enum IRNodeComponentMode
{
    WORKSPACE,
    ANNOTATION
};

enum IRNodeComponentBoundsType
{
    ORDINARY,
    RELATIVE
};


struct NodeObjectType
{
    IRNodeComponentType componentType = IRNodeComponentType::lightWeightComponent;
    IRNodeComponentMode componentMode = IRNodeComponentMode::WORKSPACE;
    NodeObjectType(){}
    
    NodeObjectType(IRNodeComponentType type)
    {
        this->componentType = type;
    }
    
    NodeObjectType(IRNodeComponentMode mode)
    {
        this->componentMode = mode;
    }

    
    NodeObjectType(IRNodeComponentType type,
                   IRNodeComponentMode mode)
    {
        this->componentType = type;
        this->componentMode = mode;
    }
};


// ===========================================================================
// IRNodeComponent
// fundamental system of the IRNodeObject
// ===========================================================================

class IRNodeComponent : public Component,
                        public IRStrComponent,
                        public ChangeBroadcaster,
                        public IRHeavyWeightComponent,
public ChangeListener
{
public:
    IRNodeComponent(Component *parent,
                    String name,
                    IRStr* str,
                    NodeObjectType objectType = NodeObjectType());
    ~IRNodeComponent();
    
    // ==================================================
    // basics
private:
    void resized() override;
    void moved() override;
    
protected:
    virtual void resizedOrMoved() {};
public:
    virtual void onResized() {};
    virtual void onMoved() {};
public:
    void setObjectCentredPosition(int x, int y);
    void setObjectBounds(Rectangle<int> bounds);
    void setObjectBounds(int x, int y, int w, int h);
    
    void setObjectTopLeftPosition(int x, int y);
    void setObjectTopLeftPositionRelative(float x, float y);
    
    void setObjectSize(int w, int h);
    
    // this method is called only when the object position and size are changed, and NOT called when its parent size is changed.
    void setObjectBoundsRelative(Rectangle<float> ratioBounds);
    void setObjectBoundsRelative(float x, float y, float w, float h);
    Rectangle<float> getObjectBoundsRelative() const { return this->relativeBoundsToParent; }
    
    IRNodeComponentBoundsType getBoundType() const { return this->boundType; }
    void setBoundType(IRNodeComponentBoundsType boundType);
    
protected:
    // Notify any change of the position and size of this object,
    // NOTE that the given argument values may NOT be represented in the actual scale but relative to the media resolution on which this object is binded.
    
    virtual void ObjectPositionChanged(int x, int y) {}
    virtual void ObjectBoundsChanged(Rectangle<int> bounds) {}
public:
    // ==================================================
    void setEnableParameters(IRNodeComponentSetUp id...);
    void setDisableParameters(IRNodeComponentSetUp id...);
    
    // managing its OpenGL context This should be called after addAndMakeVisible() of this object;
    //void initOpenGLContext();
    
    int getPreviousWidth() const;
    int getPreviousHeight() const;
    // ==================================================

    void setPreferenceWindow(PreferenceWindow* preferenceWindow);
    PreferenceWindow* getPreferenceWindow();
    // ==================================================
    // paint
    virtual void paint(Graphics& g) override;
    // ==================================================

    // managing child components binded on the NodeObject
    // this method operates following
    // # addMouseListner -> status changed by Edit mode of workspace
    // # setInterceptsMouseClicks -> status changed by Edit mode of workspace
    void childComponentManager(Component* comp);
    // ==================================================

    // Audio Source Management
    // Use addAudioComponent() to add any AudioSource made in the NodeObject
    // This method adds a given AudioSource to a mixer which connects to the global mixer to the audio output.
    void addAudioComponent(AudioSource *source);
    void removeAudioComponent(AudioSource *source);
    // Workspace uses this method to get AudioSources added in this NodeObject
    MixerAudioSource* getAudioSource() const;
    // check if any AudioSources are added in this NodeObject.
    bool isContainAudioSource() const;
    
    // ==================================================

    // interaction
    // # these JUCE oriented methods are not intended to be overriden in the IRNodeObject
    // # override mouseXXXEvent() method insted to add any interactive tasks.
private:
    void mouseDown(const MouseEvent& e) override; // JUCE oriented
    void mouseMove(const MouseEvent& e) override; // JUCE oriented
    void mouseUp(const MouseEvent& e)override; // JUCE oriented
    void mouseDoubleClick(const MouseEvent& e) override; // JUCE oriented
    void mouseDrag(const MouseEvent& e) override; // JUCE oriented
    void mouseEnter(const MouseEvent& e) override;
    void mouseExit(const MouseEvent& e) override;
    void mouseMagnify(const MouseEvent& e, float scaleFactor) override;

public:
    
    virtual void mouseUpCompleted(const MouseEvent& e) {};
    // # controlling Node Object
    void mouseDownNodeEvent(const MouseEvent& e); // make another method to avoid spaghetti codes
    void mouseMoveNodeEvent(const MouseEvent& e); // make another method to avoid spaghetti codes
    void mouseDoubleClickNodeEvent(const MouseEvent& e); // make another method to avoid spaghetti codes
    void mouseUpNodeEvent(const MouseEvent& e); // make another method to avoid spaghetti codes
    void mouseDragNodeEvent(const MouseEvent& e); // make another method to avoid spaghetti codes
    
    void pasteThisComponentEvent(const MouseEvent& e); // process of paste this Component
    void deleteThisComponentEvent(const MouseEvent& e);
    void copyThisComponentEvent(const MouseEvent& e); // process of copying this Component

    // resizing method
    // this is virtual method so that you can adjust its behavior to your NodeObject
    virtual void resizeThisComponentEvent(const MouseEvent& e);
    
    virtual void resizeTopLeftComponentEvent(const MouseEvent& e);
    virtual void resizeTopRightComponentEvent(const MouseEvent& e);
    virtual void resizeBottomLeftComponentEvent(const MouseEvent& e);
    virtual void resizeBottomRightComponentEvent(const MouseEvent& e);
    
    virtual void resizeLeftComponentEvent(const MouseEvent& e);
    virtual void resizeRightComponentEvent(const MouseEvent& e);
    virtual void resizeTopComponentEvent(const MouseEvent& e);
    virtual void resizeBottomComponentEvent(const MouseEvent& e);

    
    
    juce::Point<float> getResizingArea() const;
    void setResizingArea(juce::Point<float> area);
    void recoverEventStatus();
    
    // min max setter
    void setMinimumWidth(const float newMin);
    void setMinimumHeight(const float newMin);
    void setMaximumWidth(const float newMax);
    void setMaximumHeight(const float newMax);
    
    // ===========================================================================
    // Events for override
    // # add any codes here
    // ===========================================================================
    
    virtual void mouseDownEvent(const MouseEvent& e);
    virtual void mouseMoveEvent(const MouseEvent& e);
    virtual void mouseDoubleClickEvent(const MouseEvent& e);
    virtual void mouseUpEvent(const MouseEvent& e);
    virtual void mouseDragEvent(const MouseEvent& e);
    
    virtual void mouseEnterEvent(const MouseEvent& e);
    virtual void mouseExitEvent(const MouseEvent& e);
    virtual void mouseMagnifyEvent(const MouseEvent& e, float scaleFactor);

    void moveThisComponentEvent(const MouseEvent& e); // process of moving this Component
    
    // ===========================================================================
    // CALL BACK FUNCTIONS
    // to inform any changes to the registered object
    // advanced call back functions should be written in the IRNodeObject class not here.
    // ===========================================================================
    // change status call back for its childs class
    virtual void statusChangedWrapper(IRNodeComponentStatus status);
    // this method is used as a override function
    virtual void statusChangedCallback(IRNodeComponentStatus status);
    // callback informing
    std::function<void(IRNodeComponentStatus)> statusChangeCompleted;
    
    //called when this object is focused which means this is clicked.
    virtual void thisObjectGetFocused() {};
    
    // when file manager of this Object is updated, then all objects of IRUIFoundation
    // will share the file manger automatically.
    void updateFileManager(IRFileManager& fileManager);
    // callback informing fileManager changes
    std::function<void(IRFileManager*)> fileManagerUpdated;
    IRFileManager* getFileManager() { return FILEMANAGER; }

    // ---------------------------------------------
    // change status
    // return a flag shows whether this Component is movable or not. Default is true.
    bool isMovable() const;
    bool isVerticalMovable() const;
    bool isHorizontalMovable() const;
    // all setter
    void setMovable(bool movable, bool verticalMovable, bool horizontalMovable);
    bool isMoving() const;
    bool isDragging() const;
    // ---------------------------------------------
    // return a flag shows whether this Component is resizable or not. Default is true.
    bool isResizable() const;
    void setResizable(bool flag, bool isWidthResizableFlag, bool isHeightResizableFlag);
    void setResizable(bool flag) { setResizable(flag, flag, flag); }
    // return a flag shows whether this Component is being resized or not. Default is false.
    bool isResizing() const;
    void setResizingStart(bool flag);
    
    void showThisObject(bool flag);
    // ---------------------------------------------

    // return a flag shows whether this Component is being selected or not. Default is false.
    bool isSelected() const; // FD - THIS HAS NO IMPLEMENTATION
    void setSelected(bool flag);
    virtual void selectedChangeEvent() {}; // for IRNodeObject
    // return a flag shows whether the belonging workspace is on edit mode or not.
    bool isEditMode() const;
    virtual void editModeChangedEvent() {}; // for IRNodeObject
    
    // if edit mode is true, this object does not receive any Mouse/Keyboard events
    // if false, this object receive Mouse/Keyboard events
    void setEditMode(bool flag);
    
    bool isCopied() const;
    bool isCopiable() const;
    
    bool isMouseListenerFlag() const;
    void setMouseListenerFlag(bool flag);
    
    
    // activate
    bool isActive() const { return this->isActiveFlag; }
    void setActive(bool active);

    //unique ID
    void setUniqueID(String id);
    void setUniqueID(std::string id);

    String getUniqueID() const;
    
    // object type
    NodeObjectType getObjectType() const;
    
    // ==================================================
    //Resizing Square
    // create small square on each corner of the object square which enables to resize this object
    bool isResizingSquareInitialized = false;
   
    // called when resizing square is clicked.
private: void resizingSquareClicked(IRResizeSquare2::MovableDirection direction);
public: virtual void resizingSquareClickedAction(IRResizeSquare2::MovableDirection direction) {}

    // called when resizing square is released (mouse up event)
private: void resizingSquareReleased(IRResizeSquare2::MovableDirection direction);
public: virtual void resizingSquareReleasedAction(IRResizeSquare2::MovableDirection direction) {}
private: void resizingSquareDragged(MouseEvent e);
public: virtual void resizingSquareDraggedAction(MouseEvent e){}

    void resizingObjectFunc(IRResizeSquare2::MovableDirection direction);
    
    void setResizableMargin(Rectangle<int> margin);
    void adjustSizeToResizableArea(Rectangle<int>& bounds, const MouseEvent& e);
    void adjustCurrentXYToResizableArea(int& currentX, int& currentY, const MouseEvent& e);
    Rectangle<int> resizableMargin;
    //std::vector<IRResizeSquare* > resizingSquare;
    
    IRResizeSquare2 resizingSquare;
    void updateResizingSquare();
    void setResizingSquareColour(Colour colour);
    
    void setEnableResizingSquare(bool flag);
    bool isResizingSquareEnabled() const { return this->enableResizingSquare; }
private:
    bool enableResizingSquare = true;
    
    // ==================================================

public:
    
    // ==================================================
    // called when the object contains heavy component needs to be refreshed.
    virtual void heavyComponentRefreshed();
    // call if you want to refresh the z order of heavy components on a workspace
    virtual void heavyComponentCreatedFunc() {};
    // ==================================================

    // called when this object is moved to Front of all other objects
    virtual void moveToFrontEvent(bool isRefreshHeavyComponent, bool registerZindex) {}
    virtual void moveToBackEvent() {}
    //void bringThisToFront(bool isRefreshHeavyComponent = true, bool registerZindex = true);
public:
    void bringToFront(bool isRefreshHeavyComponent = true, bool registerZIndex = true);
    void bringToBack();

    // ==================================================
    // ### ANNOTATION ### //
    
    // in annotation mdoe, you first need to give a resolution of the media where this object is binded.
    // the bounds of this object is calculated according to the ratio ralative to the resolution and the actual size of the media object.
public:
    void setMediaResolution(int w, int h);
    void setMediaObjectSize(int w, int h);
    juce::Point<int> getMediaResolution() const { return this->mediaResolution; }
    juce::Point<float> getResolutionRatioToMedia() const { return this->resolutionRatioToMedia; }
private:
    juce::Point<int> mediaResolution;
    juce::Point<int> mediaObjectSize;
    juce::Point<float> resolutionRatioToMedia;
    // ===========================================================================
public:
    
    // object menu appreas by ctl click
    PopupMenu menu;
    // system defined popup menu events
    void defaultPopupMenuEvents();
    // user defined popup menu events
    virtual void popupMenuEvents();
    
    // object name 
    String name;

    // parent
    Component* parent = nullptr;
    
    // ============================================================
    // ==================================================
    // for algorithm
    
    // sorting...
    int sortIndex = 0;

protected:
        UserSettingStr *USERSETTING;
        IR::IRColours *SYSTEMCOLOUR;
        IRIconBank    *ICONBANK;
        IRFileManager *FILEMANAGER;
    
    
    // ============================================================
    // IRNodeObject ONLY
protected:
    virtual void ObjectPositionChanged4IRNodeObject(int x, int y) {}
    virtual void ObjectBoundsChanged4IRNodeObject(Rectangle<int> bounds) {}
    // ============================================================

private:
    
    // unique id to identify this object
    String uniqueID;
    
    // object type
    NodeObjectType objectType;
    
    // save data
    t_json saveData;
    
    // mixer for audio
    MixerAudioSource *mixer;
    // if this object contains any AudioSource
    bool containAudioSourceFlag = false;
    
   
    
    // Object appearance setup
    float minWidth = 10;
    float minHeight = 10;
    
    float maxWidth = 3000;
    float maxHeight = 3000;
    
    // for resize event
    float previousWidth = 0;
    float previousHeight = 0;
    float previousX = 0;
    float previousY = 0;
    
    IRResizeSquare2::MovableDirection resizeDirection = IRResizeSquare2::MovableDirection::None;

    // Interaction
    ComponentBoundsConstrainer constrainer;
    bool draggingFlag = false;
    Rectangle<int> draggableArea;

public:

    void setDraggableArea(Rectangle<int> area);

private:
        
    bool isResizableFlag = true;
    bool resizingFlag = false;
    bool isWidthResizableFlag = true;
    bool isHeightResizableFlag = true;
    
    bool isMovableFlag = true;
    bool isVerticalMovableFlag = true;
    bool isHorizontalMovableFlag = true;
    bool movingFlag = false;
    
    //copy related flag
    bool isCopiableFlag = true;
    bool copiedFlag = false;
    
    bool isDeletableFlag = true;
    
    bool isSelectable = true;
    bool selectedFlag = false;
    
    bool mouseListenerFlag = false;

    juce::Point<float> resizingArea;
    
    Rectangle<float> initialBounds;
    
    // default activate.
    // when deactivate, this object is hidden by moving to bihind the workspace (this is how to avoid using setVisible() causing latency)
    // used for annotater
    bool isActiveFlag = true;

    // =======================================================
    // INITIAL BOUNDS used for fixing the position and size of this object relative to the workspace
    // =======================================================

public:
    void setInitialBounds(Rectangle<float> initialBounds) { this->initialBounds = initialBounds; }
    Rectangle<float> getInitialBounds() const { return this->initialBounds; }
    

    // =======================================================

private:
    
    // global mode
    bool editModeFlag = true;
    PreferenceWindow* preferenceWindow;
    
    // ----------
    //OpenGL
    /*
    OpenGLContext openGLContext;
    void setOpenGLContextSurfaceOpacityToZero();
    
    void newOpenGLContextCreated()override
    {
    }
       
    virtual void renderOpenGL()override
    {
        OpenGLHelpers::clear(Colours::transparentBlack);
    }
    
    virtual void openGLContextClosing() override
    {
        
    }*/

    // ==================================================
    // ###### JUCE FUNCTIONS #####
    // ==================================================

    private:
        // disable JUCE size coordinate functions
    //use setObjectBounds
        void setSize(float width, float height);
    
    // use setObjectBounds
        void setBounds(Rectangle<int> bounds);
        void setBounds(int x, int y, int w, int h);
    // use setObjectCentredPosition
        void setCentredPosition(int x, int y);
        void setTopLeftPosition(int x, int y);
    
    IRNodeComponentBoundsType boundType = IRNodeComponentBoundsType::ORDINARY;
        
    Rectangle<float> relativeBoundsToParent;
    
    // for save data, Corresponding to the all parameters in ArrangeController
    IRNodeObjectStatusStr statusStr;
public:
    IRNodeObjectStatusStr* getStatusStr() { return &this->statusStr; }

    // ==================================================
    
protected:
    virtual void changeListenerCallback (ChangeBroadcaster* source) override {}


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IRNodeComponent)
};



#endif /* NodeComponent_hpp */




