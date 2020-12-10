//
//  IRVideoAnnotater.hpp
//  IRiMaS
//
//  Created by Keitaro on 07/12/2019.
//

#ifndef IRVideoAnnotater_hpp
#define IRVideoAnnotater_hpp

#include "IRStrComponent.hpp"

// DELEGATE METHODS
#include "IRVideoAnnotaterBase.h"
#include "IRVideoAnnotaterDelegate.h"

// VIDEO ANNOTATOR UI
#include "VideoTransport.h"
#include "IRVideoAnnotaterObject2.hpp"

#include "IRVideoAnnotaterSRTFile.hpp"
#include "IRJson.hpp"

#include "VideoAnnotationMenuWindow.h"
#include "VideoEventList.h"
#include "EventLogList.h"

#include "VAControllerComponent.h"

#include "VideoAnnotaterWorkspace.hpp"
#include "IRProjectOwnerBase.h"
#include "AnnotationChart.h"

// VideoAnontater object
#include "IRVATextEditorObject.h"
#include "IRVAShapeObject.h"
#include "IRVAImageViewerObject.h"
#include "IRVAWaveformObject.h"

#include "IRObjectCreater.hpp"

class IRVideoAnnotater : public IRProjectOwnerBase,
public IRVideoAnnotaterBase,
public KeyListener,
public ChangeListener,
private IRWorkspaceComponent::Listener
{
public:
    IRVideoAnnotater(IRStr* str, IRVideoAnnotaterObject2* videoPlayerObject);
    ~IRVideoAnnotater();
    
    // ==================================================
    void paint(Graphics& g) override;
    void resized() override;
    void videoResized();
    
    // do whatever necessary task when its window is opened.
    void openAnnotaterWindowAction();
    // ==================================================
    
    void initialize();
    
    // ==================================================
    // initialize the parameters ex. begin and end
    // call this method when you want to re-load another video file and reset all parameters defined for the previously loaded video file.
    void initializeAndOpenFile();
    
    void openFile(bool wantUpdateVideoWorkspace = true);
    void openFile(File file, bool wantUpdateVideoWorkspace = true);
    
    // SRTs file is the original file format
    // this covers shape, image, and other IRNodeObjects besides text subtitles.
    void openSRTs();
    void openSRTs(File file);
    void saveSRTs();
    void saveSRTs(File file);
    
    json11::Json getSRTSaveData() const;
    void setSRTSaveData(json11::Json saveData);
    
    // use this function to do any tasks after the srts file loading completed,
    // otherwise, it may cause crash
    std::function<void()> loadSRTsCompleted;
    
    // usual srt file
    void openSRT();
    // no save or export srt
    void openSRT(File file);
    
    void loadSRTAndCreateEvent(File file);
    
    // ==================================================

    std::vector<AnnotationChart::annotationData> getAnnotationData() const { return this->annotationData; }

    // ==================================================

private:
    void openDialogtoSaveSRTs();
    String savePath;
    
    bool shouldInitializeVideoSegment = true;
public:
    
    String getSRTFilePath() const { return this->srtFileLoader.getPath(); }
    String getIRSTRFilePath() const { return this->savePath; }
    
    bool hsaVideo() const;

    void myVideoLoadCompleted();
    void myVideoPlayingUpdate(double pos);
    
    void updateVideoSize(juce::Point<int> newVideoSize);
    
    // ==================================================
    void setVideoComponent(IRVideoComponent* videoComponent);
    
    
    // as videoPlayerObject is shared with IRWorkspace, you need to remove it from this component when closed.
    void createAndBindVideoPlayerObject();
    void removeVideoPlayerObject();
    
    // update video player and share the status.
    void updateVideoPlayerOfThis();
    void updateVideoPlayerOfWorkspace();
    
    void updateThisAnnotationWorkspace();
    void updateThisVideoFile();
    void updateVideoFileWorkspace();
    
    // ==================================================
    void openAnnotationMenu();
    void closeAnnotationMenu();
    
    void addEventButtonAction();
    void deleteEventButtonAction();
    // by slider
    void playPositionChangedBySliderAction();
    void playAction();
    void stopAction();
    
private:
    // video Segment slider
    void videoSegmentSliderMinChangedAction();
    void videoSegmentSliderMaxChangedAction();

    float videoSegmentBeginTime = 0;
    float videoSegmentEndTime = 0;
    
public:
    float getVideoSegmentBeginTime() const { return this->videoSegmentBeginTime; }
    float getVideoSegmentEndTime() const { return this->videoSegmentEndTime; }

    void setVideoSegmentRange(float begin, float end);
    
    // use this method when you want to set segment range initially after loading video file
    void setInitialVideoSegmentRange(float begin, float end);
    // ==================================================
   
    void setEventModifiedCallback(std::function<void(VideoAnnotationEventComponent*)> callback);
    // ==================================================

    std::vector<VideoAnnotationEventComponent*> getEventComponents()
    {
        return this->eventListComponent->getEventComponents();
    }
    // ==================================================
    //std::string getSRTFilePath();
    // ==================================================
    std::function<void()> closeAnnotationWindowCallback;
    void closeAnnotationWindow();
    // ==================================================

private:
    // ==================================================
    // SYSTEM DELEGATE
    IRVideoAnnotaterDelegate* delegate = nullptr;
    
    // ==================================================

    // VIDEO
    File videoFile;
    String path;
    
    //SRT
    String srtSavePath;
    
    bool isVideoLoaded = false;
    Rectangle<int> videoArea;
    float aspectRatio = 1.0;
    
    bool wantUpdateThisVideo = true;
    bool wantUpdateVideoWorkspace = true;

    // ==================================================

    Rectangle<int> workArea;
    // ==================================================

    // ==================================================

    IRVideoAnnotaterObject2* videoPlayerObject = nullptr;
    std::shared_ptr<IRVideoAnnotaterObject2> myVideoPlayerObject;
    
    // video annotation controller
    std::shared_ptr<VAControllerComponent> myVideoPlayerController;
    // ==================================================
    std::unique_ptr<IRVideoTransport> videoTransport;
    std::unique_ptr<VideoAnnotationMenuWindow> annotationMenu;
    
    void createVideoTransport();
    // ==================================================
    
    std::shared_ptr<VideoEventList> eventListComponent;
    void createEventListComponent();

    void clearAllEventComponent();
    void clearEventComponent(VideoAnnotationEventComponent* eventComponent);
    void addEventComponent(VideoAnnotationEventComponent* eventComponent);
    void deleteSelectedEvents();
    void deleteEventComponent(VideoAnnotationEventComponent* event);
    
    void createEventComponent(IRNodeObject* obj, bool addEventList = true);
    void createTextEventComponent();
    void createTextEventComponentFromSRT(SubtitleItem* item);
    void createTextEventComponentFromIRNodeObject(IRNodeObject* obj, bool addEventList);
    void createShapeEventComponent();
    void createShapeEventComponentFromNodeObject(IRNodeObject* obj, bool addEventList);
    void createImageEventComponent();
    void createImageEventComponentFromNodeObject(IRNodeObject* obj, bool addEventList);
    void createAudioEventComponent();
    void createAudioEventComponentFromNodeObject(IRNodeObject* obj, bool addEventList);

    void eventComponentResized();
    
    void setupEventComponent(IRNodeObject* obj, VideoAnnotationEventComponent* event, bool addEventList = true);
    
    void setupSRTTextEventComponent(IRNodeObject* obj, VideoAnnotationEventComponent* event, bool addEventList = true);

    void duplicateNodeObject(IRNodeObject* obj, bool addEventList = true);
    
    // chart
    std::vector<AnnotationChart::annotationData> annotationData;
    bool enableUpdateAnnotationData = true;
    
    
    void updateAnnotationData();
    // ==================================================
    //IRWorkspace
    // getting to know when the selection status of a NodeObject has been changed.
    // This function is called when an object is either selected or unselected.
    // this function is, for instance, used to update the Object Controller in the IRRightBar
    void nothingSelected() override;
    void nodeObjectPasted(IRNodeObject* obj) override;
    void nodeObjectWillDeleted(IRNodeObject* obj) override;
    // workspaceComponent::Listener
    void nodeObjectSelectionChange(IRNodeObject* obj) override;
    void nodeObjectGetFocused(IRNodeObject* obj)override;
    void editModeChanged(IRWorkspaceComponent* changedSpace) override;
    void heavyObjectCreated(IRNodeObject* obj) override;
    
    
    void deselectAllObjectsOnWorkspace();
    // ==================================================
    std::shared_ptr<EventLogList> eventLogList;
    
    

    // ==================================================
    // Apply
    void applyEventsOnTheLoadedVideo(VideoAnnotationEventComponent* event);
    void updateEventsOnTheLoadedVideo();
    // create
    void createEventOnTheLoadedVideo(VideoAnnotationEventComponent* event);
    void deleteEventOnTheLoadedVideo();
    // ==================================================

    void changeListenerCallback (ChangeBroadcaster* source) override;
    void videoTransportChangeListener();
    void annotationMenuChangeListener();
    void VAControllerChangeListener();
    
    // ==================================================

    // IRVideoAnnotaterBase
    void eventModifiedAction(Component* modifiedEvent) override;
    void eventSelectedAction(Component* selectedEvent) override;
    
    void updateAnnotation() override;
    void showEventPosition(Component* event) override;
    void eventActivationChanged(Component* changedEvent) override;
    // ==================================================

    std::function<void(VideoAnnotationEventComponent*)> eventModifiedCallback;
    // ==================================================
    
    void textEventComponentSelected(VideoAnnotationEventComponent* event);
    void shapeEventComponentSelected(VideoAnnotationEventComponent* event);
    
    // ==================================================
    //SRTs
    
    void loadAndApplySRTs();
    
    srtWriter srt;
    srtLoader srtL;
    IRVideoAnnotaterSRTFile srtFileLoader;
    
    //Json
    
    IRJson jsonManager;
    
    // ==================================================


    bool keyPressed(const KeyPress& key, Component* originatingComponent) override;
    
    void DeleteKeyPressed();
    void AKeyPressed();
    void CommandWKeyPressed();
    void CommandEPressed();
    void CommandSPressed();
    // ==================================================
    
    std::shared_ptr<IRStr> ir_str;
    IRStr* ir_parentStr = nullptr;
    // ==================================================

    std::shared_ptr<VideoAnnotaterWorkspace> workspace;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IRVideoAnnotater)

};

#endif /* IRVideoAnnotater_hpp */
