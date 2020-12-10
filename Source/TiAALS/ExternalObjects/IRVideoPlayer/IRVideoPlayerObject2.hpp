//
//  IRVideoPlayerObject22.hpp
//  NodeComponentObject_Study - App
//
//  Created by Keitaro on 28/01/2019.
//

#ifndef IRVideoPlayerObject2_hpp
#define IRVideoPlayerObject2_hpp

#include "IRNodeObject.hpp"
#include "IRVideoPlayer.h"
#include "IRNodeObjectWorkspace.hpp"
#include "IRTextEditorObject.hpp"
//#include "IROnVideoPlayerController.hpp"
#include "IROnVideoPlayerControllerWindow.h"

class IRVideoPlayerObject2 : public IRNodeObject

{
    
public:
    
    IRVideoPlayerObject2(Component* parent, IRStr* str, bool withOpenButton = true);
    ~IRVideoPlayerObject2();
    
    // --------------------------------------------------
    // copy related methods
    
    virtual IRNodeObject* copyThisObject() override;
    
    virtual IRNodeObject* copyContents(IRNodeObject* object) override;
    
    virtual IRNodeObject* copyDragDropContents(IRNodeObject* object) override;
    
    // share object contents owned by this obejct to twithObject
    void shareContentsWith(IRVideoPlayerObject2* withObject);
    
    // --------------------------------------------------
    
    virtual t_json saveThisToSaveData() override;
    
    // --------------------------------------------------
    
    virtual void loadThisFromSaveData(t_json data) override;
    
    // --------------------------------------------------
    // resize reated methods
    virtual void resized() override;
    
    
    void setBoundsInWorkspace(Rectangle<int> rect) { this->boundsInWorkspace = rect; }
    Rectangle<int> getBoundsInWorkspace() const { return this->boundsInWorkspace; }
    
    // --------------------------------------------------
    
    void mouseUpEvent(const MouseEvent& e) override;

    // --------------------------------------------------

    void paint(Graphics& g) override;
    
    // --------------------------------------------------
    // define if call videoLoadCompletedCallbackFunc();
    void openFile(File file, bool isCallback = true);
    void openFile(bool isCallbback = true);

    std::function<void()> videoLoadCompletedCallbackFunc;
    virtual void videoPlayingUpdateVirtualFunc(double pos) {}
    std::function<void(double)> videoPlayingUpdateCallbackFunc;
    // --------------------------------------------------
    void play();
    void stop();
    void setPlayPosition(double newPositionInSec);
    
    juce::Point<int> getVideoSize();
    // --------------------------------------------------

    IRVideoPlayer* getVideoPlayer() { return this->videoPlayer.get(); }

    // --------------------------------------------------
    void enableController(bool flag);
    // --------------------------------------------------
    
    // --------------------------------------------------


    // refresh OpenGL when added
    void heavyComponentRefreshed() override;
    // --------------------------------------------------

    void setVideoPlayerController(IROnVideoPlayerController* playerController);
    bool hasVideoPlayerController() const { return this->hasVideoPlayerControllerFlag; }
    // --------------------------------------------------

    std::shared_ptr<IRVideoPlayer> videoPlayer;
    void playAction();
    void pauseAction();
    void playPositionChangedAction();
    
    void changePlayPosition(float pos);
    
    void resizeThisComponentEvent(const MouseEvent& e) override;
    void resizeThisComponent(Rectangle<int> rect);
    
    
private:
    
    void IRChangeListenerCallback(ChangeBroadcaster* source) override;
    void playControllerChangeListenerCallback();
    // --------------------------------------------------
    IROnVideoPlayerController* playerController = nullptr;
    bool hasVideoPlayerControllerFlag = false;
    
    // --------------------------------------------------
    // ResizingSquare

    void resizingSquareClickedAction(IRResizeSquare2::MovableDirection direction) override;
    void resizingSquareReleasedAction(IRResizeSquare2::MovableDirection direction) override;
    void resizingSquareDraggedAction(MouseEvent e) override;
    
    // --------------------------------------------------
    // call back function by IRVideoPlayer
    // use videoLoadCompletedCallback() to overload
    void videoLoadCompletedAction();
    void videoPlayingUpdateAction(double pos);
    
    // --------------------------------------------------
    void refreshZIndex();
    
    // --------------------------------------------------
    bool enableControllerFlag = true;
    // --------------------------------------------------

    void statusChangedCallback(IRNodeComponentStatus status) override;
    // --------------------------------------------------

    Rectangle<int> boundsInWorkspace;
    // --------------------------------------------------

    // called when this object is brought to the most Front of all other objects
    void moveToFrontAction() override;
    // --------------------------------------------------
    
    float rangeBegin = 0;
    float rangeEnd = 10;
    
public:
    void setRangeBegin(float begin);
    void setRangeEnd(float end);
    float getRangeBegin() const { return this->rangeBegin;}
    float getRangeEnd() const { return this->rangeEnd; }
    // --------------------------------------------------
    double getVideoLength()
    {
        if(getVideoPlayer() != nullptr)
            return this->getVideoPlayer()->getVideoLength();
        else return 0;
    }

    // --------------------------------------------------
    // call videoLoadCompletedCallbackFunc if true
    bool isCallback = true;
    
    bool resizing = false;
    // IRVideoPlayer *videoPlayer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IRVideoPlayerObject2)
};

#endif /* IRVideoPlayerObject2_hpp */
