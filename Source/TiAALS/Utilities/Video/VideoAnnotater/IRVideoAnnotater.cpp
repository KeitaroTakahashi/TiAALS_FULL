//
//  IRVideoAnnotater.cpp
//  IRiMaS
//
//  Created by Keitaro on 07/12/2019.
//

#include "IRVideoAnnotater.hpp"

IRVideoAnnotater::IRVideoAnnotater(IRStr* str, IRVideoAnnotaterObject2* videoPlayerObject) :
videoArea(10, 10, 640, 480),
videoPlayerObject(videoPlayerObject),
ir_parentStr(str)
{
    setWantsKeyboardFocus(true);
    //addKeyListener(this);
    
    initialize();
    
    // setup delegate
    this->delegate = new IRVideoAnnotaterDelegate(this);

    createVideoTransport();
    createEventListComponent();
    this->eventLogList.reset(new EventLogList(str));
    addAndMakeVisible(this->eventLogList.get());
    

}

IRVideoAnnotater::~IRVideoAnnotater()
{
    closeAnnotationMenu();
            
    this->myVideoPlayerObject.reset();

    this->eventListComponent.reset();
    
    delete this->delegate;
    
}


// ==================================================
void IRVideoAnnotater::paint(Graphics& g)
{
    g.fillAll(this->ir_str->SYSTEMCOLOUR.fundamental);
    g.setColour(this->ir_str->SYSTEMCOLOUR.contents);
    g.fillRect(this->videoArea);
    g.setColour(this->ir_str->SYSTEMCOLOUR.contents);
    g.fillRect(this->workArea);
    
}

void IRVideoAnnotater::resized()
{
    int xMarge = 10;
    int yMarge = 10;
    
    int eventLogListWidth = 350;
    this->videoArea = Rectangle<int>(xMarge, yMarge, getWidth() - eventLogListWidth, getHeight() * 0.5);
    int ha = this->videoArea.getY() + this->videoArea.getHeight() + 5;
    
    this->workArea  = Rectangle<int>(xMarge,
                                     ha + 90,
                                     getWidth() - 20,
                                     getHeight() - (ha + 100));
    
    videoResized();
    
    this->videoTransport->setBounds(xMarge,
                                   ha,
                                   getWidth() - 20,
                                   80);
    
    //this->openVideoButton.setBounds(this->videoArea);
    
    this->eventListComponent->setBounds(this->workArea);
  
   
    this->eventLogList->setBounds(xMarge + xMarge+ this->videoArea.getWidth(),
                                  yMarge,
                                  eventLogListWidth - xMarge - xMarge,
                                  getHeight() * 0.5);
    

    
    eventComponentResized();
}

void IRVideoAnnotater::videoResized()
{
    if(this->myVideoPlayerObject.get() != nullptr)
    {
        
        //std::cout << "IRVideoAnnotater::videoResized\n";
        this->myVideoPlayerObject->resizeAndCentredThisComponent(this->videoArea);
        
       
        /*
        auto vp = this->myVideoPlayerObject->getVideoPlayerObject();
        
        vp->resizeThisComponent(Rectangle<int>(0, 0,
                                               this->videoArea.getWidth(),
                                               this->videoArea.getHeight()  ));
        */

        //this->workspace->setBounds(this->myVideoPlayerObject->getBounds());
        
    }
}
// ==================================================



void IRVideoAnnotater::initialize()
{
    
    //keyListener setup
    //setWantsKeyboardFocus(true);
    //addKeyListener(this);
    
    // create IRStr
    this->ir_str.reset(new IRStr());
    this->ir_str->projectOwner = this;
    this->ir_str->parentStr = this->ir_parentStr;
    this->ir_str->setKeyListener(this);
    this->ir_str->setMouseListener(this);
    this->ir_str->projectName = "VideoAnnotator";
    this->ir_str->SYSTEMCOLOUR = this->ir_parentStr->SYSTEMCOLOUR;
    this->ir_str->ICONBANK = this->ir_parentStr->ICONBANK;
    
    Font f;
    this->ir_str->fontFamilyList = f.findAllTypefaceNames();

}
// ==================================================

void IRVideoAnnotater::openAnnotaterWindowAction()
{
    if(this->myVideoPlayerObject.get() != nullptr)
    {
        this->myVideoPlayerObject->moveToFrontAction();
    }
}

// ==================================================

void IRVideoAnnotater::createVideoTransport()
{
    // video transport
    this->videoTransport.reset( new IRVideoTransport(this->ir_str.get(), this));
    addAndMakeVisible(this->videoTransport.get());
    this->videoTransport->addChangeListener(this);
    
    this->videoTransport->videoSegmentMinChangedCallback = [this]{videoSegmentSliderMinChangedAction();};
    this->videoTransport->videoSegmentMaxChangedCallback = [this]{videoSegmentSliderMaxChangedAction();};

    setVisible(true);
}

void IRVideoAnnotater::createEventListComponent()
{
    // eventListComponent
    this->eventListComponent.reset(new VideoEventList(this->ir_str.get(), this));
    addAndMakeVisible(this->eventListComponent.get());

}

void IRVideoAnnotater::nothingSelected()
{
    if(this->eventLogList.get() == nullptr) return;
    
    this->eventLogList->removeLogComponent();
    
    if(this->myVideoPlayerObject.get() != nullptr)
    {
        if(this->myVideoPlayerObject->hasVideo())
        {
        
            if(this->myVideoPlayerController.get() != nullptr)
                this->eventLogList->setLogComponent(this->myVideoPlayerController.get());
        }
    }
        
}

void IRVideoAnnotater::nodeObjectPasted(IRNodeObject* obj)
{
    String name = obj->name;
    std::cout << "nodeObjectPasted " << name << std::endl;
    
    duplicateNodeObject(obj);
}

void IRVideoAnnotater::nodeObjectWillDeleted(IRNodeObject* obj)
{
    auto event = static_cast<VideoAnnotationEventComponent* >(obj->getEventComponent());
    
    std::cout << obj << " nodeObejctWillDeleted , event delete " << event << std::endl;
    deleteEventComponent(event);
}
// ==================================================

void IRVideoAnnotater::eventComponentResized()
{

}

// ==================================================

void IRVideoAnnotater::createAndBindVideoPlayerObject()
{
    if(this->videoPlayerObject != nullptr)
    {
        std::cout << "IRVideoAnnotater create myVideoPlayerObject\n";
        
        this->myVideoPlayerObject.reset( new IRVideoAnnotaterObject2(
                                                                    this,
                                                                     this->ir_str.get(),
                                                                    false));
        this->myVideoPlayerObject->getWorkspace()->enableDrawGrids(true);
        this->myVideoPlayerObject->getWorkspace()->addListener(this);
        addKeyListener(this->myVideoPlayerObject->getWorkspace());

        this->myVideoPlayerObject->videoLoadCompletedCallbackFunc = [this] { myVideoLoadCompleted(); };
        this->myVideoPlayerObject->videoPlayingUpdateCallbackFunc = [this](double pos){ myVideoPlayingUpdate(pos); };
        // disable QT controller
        this->myVideoPlayerObject->enableController(false);

        
        // annotater reference video is not selectable.
        this->myVideoPlayerObject->setEnableResizingSquare(false);
        this->myVideoPlayerObject->setMovable(false, false, false);
        this->myVideoPlayerObject->setResizable(false);
        this->myVideoPlayerObject->setObjectBounds(this->videoArea);
        addAndMakeVisible(this->myVideoPlayerObject.get());
        
        // if parent object on the workspace already has a video file, then load it
        auto vp = this->videoPlayerObject->getVideoPlayerObject()->getVideoPlayer();
        if(this->videoPlayerObject->hasVideo())
        {
            File f = vp->getMovieFile();
            // update myVideoPlayerObject without notification = false
            this->myVideoPlayerObject->openFile(f, false);
        }
        
        auto v = this->myVideoPlayerObject->getVideoPlayerObject()->getVideoPlayer();
        
        this->myVideoPlayerObject->setFixObjectSizeRatioWithOriginalSize(false, v->getCurrentVideoBounds().toFloat());
        
        // create annotation controller for myVideoPlayerObject
        this->myVideoPlayerController.reset(new VAControllerComponent(this->ir_str.get()));
        this->myVideoPlayerController->addChangeListener(this);
        
        resized();
    }
}
void IRVideoAnnotater::removeVideoPlayerObject()
{
    if(this->myVideoPlayerObject.get() != nullptr)
    {
        removeChildComponent(this->myVideoPlayerObject.get());
    }
}
// ==================================================

// --------------------------------------------------
void IRVideoAnnotater::openFile(bool wantUpdateVideoWorkspace)
{
    if(this->myVideoPlayerObject == nullptr) return ;
    
    // update video on the workspace
    this->wantUpdateVideoWorkspace = wantUpdateVideoWorkspace;
    
    this->myVideoPlayerObject->getVideoPlayerObject()->openFile();
    
    resized();
}

void IRVideoAnnotater::openFile(File file, bool wantUpdateVideoWorkspace)
{
    if(this->myVideoPlayerObject == nullptr) return ;
    
    // update video on the workspace
    this->wantUpdateVideoWorkspace = wantUpdateVideoWorkspace;
    
    this->myVideoPlayerObject->getVideoPlayerObject()->openFile(file);
    resized();
}

void IRVideoAnnotater::initializeAndOpenFile()
{
    this->shouldInitializeVideoSegment = true;
    openFile();
}
// --------------------------------------------------

void IRVideoAnnotater::openSRTs()
{
    FileChooser chooser("Select a SRT file to load...",
                        {},
                        "*.srt", "*.srts");
    
    if(chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        //this->SrtPath = file.getFullPathName();
        this->savePath = file.getFullPathName();
        this->jsonManager.readSaveData(file.getFullPathName().toStdString());
        loadAndApplySRTs();
        
    }
}

void IRVideoAnnotater::openSRTs(File file)
{
    if(!file.exists())
    {
        KLib().showErrorMessage("Error : IRVideoAnnotator openSRTs() : Could not find " + file.getFullPathName() + " : file not exists!");
        return;
    }
    this->jsonManager.readSaveData(file.getFullPathName().toStdString());

    loadAndApplySRTs();
}


void IRVideoAnnotater::saveSRTs()
{
    if(this->srtSavePath.length() == 0)
    {
        openDialogtoSaveSRTs();
        
    }else{
        File file(this->srtSavePath);
        if(file.exists())
        {
            saveSRTs(file);
        }else{
            this->srtSavePath = "";
            saveSRTs();
        }
    }
}

void IRVideoAnnotater::saveSRTs(File file)
{

    json11::Json wsData;
    auto workspaces = this->myVideoPlayerObject->getWorkspace();
    wsData = workspaces->makeSaveDataOfThis();
    
    json11::Json saveData = json11::Json::object({
        {"IRVideoAnnotaterSaveData", wsData}
    });
    this->jsonManager.setData(saveData);
    this->jsonManager.writeSaveData(file.getFullPathName().toStdString());
  
    
}

void IRVideoAnnotater::openDialogtoSaveSRTs()
{
    FileChooser chooser("Save irSrt file...",
                        {},
                        "");
    if(chooser.browseForFileToSave(true))
    {
        auto file = chooser.getResult();
        this->savePath = file.getFullPathName() + ".srt";
        saveSRTs(File(this->savePath));
    }
}

json11::Json IRVideoAnnotater::getSRTSaveData() const
{
    json11::Json wsData;
    auto workspaces = this->myVideoPlayerObject->getWorkspace();
    wsData = workspaces->makeSaveDataOfThis();
    
    json11::Json saveData = json11::Json::object({
        {"IRVideoAnnotaterSaveData", wsData}
    });
    
    return saveData;
}

void IRVideoAnnotater::setSRTSaveData(json11::Json saveData)
{
    this->jsonManager.setData(saveData);
    loadAndApplySRTs();
}

// srt
void IRVideoAnnotater::openSRT()
{
    FileChooser chooser("Select a SRT file to load...",
                        {},
                        "*.srt", "*.srts");
    
    if(chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        this->savePath = file.getFullPathName();
        openSRT(file);
    }
}

void IRVideoAnnotater::openSRT(File file)
{
    if(!file.exists())
    {
        KLib().showErrorMessage("Error : IRVideoAnnotator openSRT() : Could not find " + file.getFullPathName() + " : file not exists!");
        return;
    }
    
    loadSRTAndCreateEvent(file);
}

void IRVideoAnnotater::loadSRTAndCreateEvent(File file)
{
    std::string filePath = file.getFullPathName().toStdString();
    this->srtL.openFile(filePath);
    auto eventList = this->srtL.getSubtitleItems();
    
    if(eventList.size() == 0)
    {
        KLib().showErrorMessage("SRT file has no contents to read.");
        return;
    }
        
    for(auto item : eventList)
    {
        createTextEventComponentFromSRT(item);
    }
    
    this->myVideoPlayerObject->setPlayPosition(0.0);
    this->videoTransport->setCurrentPlayingPosition(0.0);
}


// --------------------------------------------------

bool IRVideoAnnotater::hsaVideo() const { return this->isVideoLoaded; }

// --------------------------------------------------

void IRVideoAnnotater::myVideoLoadCompleted()
{
    
    videoResized();
    
    double len = this->myVideoPlayerObject->getVideoPlayerObject()->getVideoPlayer()->getVideoLength();
    
    std::cout << "loaded video length = " << len << std::endl;
    
    
    this->videoTransport->setVideoLengthInSec(len);

    if(!this->shouldInitializeVideoSegment)
    {
        this->videoTransport->setVideoRange(this->videoSegmentBeginTime,
                                            this->videoSegmentEndTime);
        
        this->shouldInitializeVideoSegment = true;
    }else{
        setVideoSegmentRange(0, len);
    }
        
    videoResized();
    
    // update video on the workspace
    if(this->wantUpdateVideoWorkspace)
    {
        updateVideoFileWorkspace();
        //reset
        this->wantUpdateVideoWorkspace = false;
    }
    
    
    
    std::cout << "myVideoPLater set play position to 0.0\n";
    this->myVideoPlayerObject->setPlayPosition(0.0);
    this->videoTransport->setCurrentPlayingPosition(0.0);

}

void IRVideoAnnotater::updateVideoSize(juce::Point<int> newVideoSize)
{
    setVideoSize(newVideoSize);
}

void IRVideoAnnotater::myVideoPlayingUpdate(double pos)
{
    this->videoTransport->setCurrentPlayingPosition(pos);
    
}


// ==================================================

void IRVideoAnnotater::openAnnotationMenu ()
{
    std::cout << "openAnnotationMenu\n";
    Rectangle<int> pos = getBounds();
    pos.setX(getScreenPosition().getX());
    pos.setY(getScreenPosition().getY());

    this->annotationMenu.reset(new VideoAnnotationMenuWindow(this->ir_str.get(),
                                                             "AnnotationMenu",
                                                             pos));
    
    this->annotationMenu->closeMenuWindow = [this]{ closeAnnotationMenu(); };
    //this->annotationMenu->setChangeEventListener(this);
    this->annotationMenu->addChangeListener(this);
}

void IRVideoAnnotater::closeAnnotationMenu ()
{
    this->annotationMenu.reset();
}

void IRVideoAnnotater::addEventButtonAction ()
{
    openAnnotationMenu();
}

void IRVideoAnnotater::deleteEventButtonAction ()
{
    deleteSelectedEvents();
}

void IRVideoAnnotater::playPositionChangedBySliderAction()
{
    stopAction();
    
    float p = this->videoTransport->getPlayPosition();
    this->myVideoPlayerObject->setPlayPosition(p);
    
    //test
    //this->videoPlayerObject->getWorkspace()->getVideoPlayerObject()->changePlayPosition(p);

}

void IRVideoAnnotater::playAction()
{
   // this->workspace->setEditMode(false);
    this->myVideoPlayerObject->play();
}
void IRVideoAnnotater::stopAction()
{
    //this->workspace->setEditMode(true);
    this->myVideoPlayerObject->stop();

}

// ==================================================


void IRVideoAnnotater::videoSegmentSliderMinChangedAction()
{
    auto v = this->videoTransport->getSegmentValue();
    this->videoSegmentBeginTime = v[0];
    this->videoSegmentEndTime = v[1];
    
    std::cout << "videoSegmentSliderMinChangedAction ; " << this->videoSegmentBeginTime << ", " << this->videoSegmentEndTime << std::endl;
    
    this->myVideoPlayerObject->setVideoRange(v[0],
                                             v[1], true);
    this->videoPlayerObject->setVideoRange(v[0],
                                           v[1], true);
}
void IRVideoAnnotater::videoSegmentSliderMaxChangedAction()
{
    auto v = this->videoTransport->getSegmentValue();
    this->videoSegmentBeginTime = v[0];
    this->videoSegmentEndTime = v[1];
    
    std::cout << "videoSegmentSliderMinChangedAction ; " << this->videoSegmentBeginTime << ", " << this->videoSegmentEndTime << std::endl;

    
    this->myVideoPlayerObject->setVideoRange(v[0], v[1], false);
    this->videoPlayerObject->setVideoRange(v[0], v[1], false);
}

void IRVideoAnnotater::setVideoSegmentRange(float begin, float end)
{
    this->videoSegmentBeginTime = begin;
    this->videoSegmentEndTime = end;
    
    this->videoTransport->setVideoRange(begin, end);
    
}

void IRVideoAnnotater::setInitialVideoSegmentRange(float begin, float end)
{
    this->videoSegmentBeginTime = begin;
    this->videoSegmentEndTime = end;
    this->shouldInitializeVideoSegment = false;
}

// ==================================================

void IRVideoAnnotater::changeListenerCallback (ChangeBroadcaster* source)
{
    if(source == this->videoTransport.get())
    {
        videoTransportChangeListener();
    }
    
    if(this->annotationMenu.get() != nullptr)
    {
        if(source == this->annotationMenu.get())
        {
            annotationMenuChangeListener();
        }
    }
    
    if(this->myVideoPlayerController.get() != nullptr)
    {
        if(source == this->myVideoPlayerController.get())
        {
            VAControllerChangeListener();
        }
    }
    
}

void IRVideoAnnotater::setEventModifiedCallback(std::function<void(VideoAnnotationEventComponent*)> callback)
{
    this->eventModifiedCallback = callback;
}

// ==================================================

void IRVideoAnnotater::eventModifiedAction(Component* modifiedEvent)
{
    std::cout << "IRVideoAnnotater::eventModifiedAction\n";
    auto event = dynamic_cast<VideoAnnotationEventComponent* >(modifiedEvent);
    jassert(event != nullptr);
    
    // stop playing video first
    //this->videoPlayerObject->stop();
    //this->myVideoPlayerObject->stop();
    
    //resize to sort
    auto currentPosition = this->eventListComponent->getViewPosition();
    this->eventListComponent->resized();
    this->eventListComponent->setViewPosition(currentPosition);

}

void IRVideoAnnotater::eventSelectedAction(Component* selectedEvent)
{
    std::cout << "selectedEvent = " << selectedEvent << std::endl;
    auto event = static_cast<VideoAnnotationEventComponent* >(selectedEvent);
    jassert(event != nullptr);
    
    // select object
    //this->workspace->deselectAllObjects();
    std::cout<< "nodeobj = " << event->getNodeObject() << std::endl;
    /*
    if(this->workspace->isEditMode())
    {
        event->getNodeObject()->setSelected(true);
    }*/
    using s = VideoAnnotationEventComponent::VideoAnnotationType;
       switch (event->getType())
       {
           case s::TEXT:
               textEventComponentSelected(event);
               break;
           case s::SHAPE:
               shapeEventComponentSelected(event);
               break;
           case s::IMAGE:
               break;
           case s::AUDIO:
               break;
           default:
               break;
       }
    
    
    //this->videotransport->setCurrentPlayingPosition
}

void IRVideoAnnotater::showEventPosition(Component* event)
{
    // first deselect all objects
    deselectAllObjectsOnWorkspace();
    
    auto e = static_cast<VideoAnnotationEventComponent* >(event);
    
    this->videoTransport->setCurrentPlayingPosition(e->getBeginTimeCode());
    
    this->myVideoPlayerObject->getWorkspace()->setCurrentTimeCode(e->getBeginTimeCode());
    this->myVideoPlayerObject->setPlayPosition(e->getBeginTimeCode());
    
    e->getNodeObject()->setSelected(true);

}

void IRVideoAnnotater::eventActivationChanged(Component* changedEvent)
{
    
    std::cout << "eventActivationChanged\n";
    //auto e = static_cast<VideoAnnotationEventComponent* >(changedEvent);
    float p = this->videoTransport->getPlayPosition();

    this->myVideoPlayerObject->getWorkspace()->updateVideoPlayingPos(p);
}

// ==================================================

void IRVideoAnnotater::textEventComponentSelected(VideoAnnotationEventComponent* event)
{
    //auto e = static_cast<AnnotationTextEventComponent* >(event);
    
    //auto c = static_cast<IRVideoAnnotationTextComponent*>(this->myVideoPlayerObject->getVideoAnnotationComponentOf(event));
    //this->eventLogList->setLogComponent(c->getIRTextEditorObject()->getObjController());
}

void IRVideoAnnotater::shapeEventComponentSelected(VideoAnnotationEventComponent* event)
{
    auto e = static_cast<AnnotationShapeEventComponent* >(event);

}

void IRVideoAnnotater::videoTransportChangeListener()
{
    auto status = this->videoTransport->getStatus();
    switch (status)
    {
        case IRVideoTransport::OpenVideoFile:
            initializeAndOpenFile();
            break;
        case IRVideoTransport::OpenAnnotationFile:
            openSRTs();
            break;
        case IRVideoTransport::SaveAnnotationFile:
            saveSRTs();
            break;
        case IRVideoTransport::addEventButtonClicked:
            addEventButtonAction();
            break;
        case IRVideoTransport::deleteEventButtonClicked:
            deleteEventButtonAction();
            break;
        case IRVideoTransport::play:
            playAction();
            break;
        case IRVideoTransport::stop:
            stopAction();
            break;
        case IRVideoTransport::playPositionChanged:
            playPositionChangedBySliderAction();
            break;
        case IRVideoTransport::videoSegmentMinChanged:
            // DO NOT CALL ANY METHOD NEEDS TO BE SYNCRONIZED
            //videoSegmentSliderMinChangedAction();
            break;
        case IRVideoTransport::videoSegmentMaxChanged:
            // DO NOT CALL ANY METHOD NEEDS TO BE SYNCRONIZED
            //videoSegmentSliderMaxChangedAction();
            break;
        default:
            break;
    }
}

void IRVideoAnnotater::annotationMenuChangeListener()
{
    using s = VideoAnnotationMenuWindow::VideoAnnotationStatus;
    switch (this->annotationMenu->getStatus())
    {
        case s::CreateTextEvent:
            createTextEventComponent();
            break;
        case s::CreateShapeEvent:
            createShapeEventComponent();
            break;
        case s::CreateImageEvent:
            createImageEventComponent();
            break;
        case s::CreateAudioEvent:
            createAudioEventComponent();
            break;
        default:
            break;
    }
}

void IRVideoAnnotater::VAControllerChangeListener()
{
    using s = VAControllerComponent::VAControllerStatus;
    switch(this->myVideoPlayerController->getStatus())
    {
        case s::OpenVideoFile:
            initializeAndOpenFile();
            break;
        case s::OpenAnnotationFile:
            openSRTs();
            break;
        case s::SaveAnnotationFile:
            saveSRTs();
            break;
        case s::OpenSRTFile:
            openSRT();
            break;
        case s::SaveSRTFile:
            break;
        default:
            break;
    }
}


// ==================================================
void IRVideoAnnotater::clearAllEventComponent()
{
    this->eventListComponent->clearAllEventComponent();
}

void IRVideoAnnotater::clearEventComponent(VideoAnnotationEventComponent* eventComponent)
{

}

void IRVideoAnnotater::addEventComponent(VideoAnnotationEventComponent* eventComponent)
{
    this->eventListComponent->createEventComponent(eventComponent);
    
    // call event
    eventSelectedAction(eventComponent);
}

void IRVideoAnnotater::deleteSelectedEvents()
{
    auto events = this->eventListComponent->getSelectedEventComponents();
    
    for(auto e : events)
    {
        std::cout << "Selected List component = " << e << std::endl;
        if(e->getNodeObject()->isSelected())
        {
            this->eventLogList->removeLogComponent();
        }
        
        deleteEventComponent(e);
        //this->eventListComponent->deleteEventComponent(e);
        
        
        //this->workspace->deleteObject(e->getNodeObject());
    }
    
    

    //this->eventListComponent->deleteSelectedEventComponent();
    
    //deleteEventOnTheLoadedVideo();
    //eventModifiedAction();
}

void IRVideoAnnotater::deleteEventComponent(VideoAnnotationEventComponent* event)
{
    
    this->eventListComponent->deleteEventComponent(event);

}

// ==================================================

// ==================================================

void IRVideoAnnotater::closeAnnotationWindow()
{
    if(this->closeAnnotationWindowCallback != nullptr)
        this->closeAnnotationWindowCallback();
}

// ==================================================

void IRVideoAnnotater::updateThisAnnotationWorkspace()
{
    if(this->videoPlayerObject == nullptr || this->myVideoPlayerObject.get() == nullptr) return;
    
    
    
}

void IRVideoAnnotater::updateThisVideoFile()
{
    if(this->videoPlayerObject == nullptr || this->myVideoPlayerObject.get() == nullptr) return;

    File f = this->videoPlayerObject->getVideoPlayerObject()->getVideoPlayer()->getMovieFile();
    
    if(f.exists()) this->myVideoPlayerObject->openFile(f);
    else std::cout << "IRVideoAnnotater::updateThisVideoFile file does not exist!" << std::endl;

}

void IRVideoAnnotater::updateVideoFileWorkspace()
{
    if(this->videoPlayerObject == nullptr || this->myVideoPlayerObject.get() == nullptr) return;

    File f = this->myVideoPlayerObject->getVideoPlayerObject()->getVideoPlayer()->getMovieFile();
    this->videoPlayerObject->openFile(f);
}
// ==================================================

void IRVideoAnnotater::updateVideoPlayerOfThis()
{
    if(this->videoPlayerObject != nullptr)
    {
        if(this->myVideoPlayerObject.get() != nullptr)
        {
            updateThisVideoFile();
        }
    }
}
void IRVideoAnnotater::updateVideoPlayerOfWorkspace()
{
    // #!!!!! some inefficient repeat happening. check by printing process....
    
    if(this->videoPlayerObject != nullptr && this->myVideoPlayerObject.get() != nullptr)
    {
        
        auto mySpace = this->myVideoPlayerObject->getWorkspace();
        auto space = this->videoPlayerObject->getWorkspace();

        space->initializeWorkspace();
        
        auto wsData = mySpace->makeSaveDataOfThis();
        
        json11::Json saveData = json11::Json::object({
            {"IRVideoAnnotaterSaveData", wsData}
        });
                
        space->loadAndApplyIRSRT(saveData);
        
        // for each nodeObject of space, create an event component and copy the data
        for(auto o : space->getObjectList())
        {
            std::cout << "creating object of " << o->name << " at " << o->getStartTimeSec() << " to " << o->getEndTimeSec() << std::endl;
            createEventComponent(o, false);
        }

        space->setEditMode(false);
        
        updateAnnotationData();
        
        this->videoPlayerObject->getPlayerController()->setAnnotationData(this->annotationData);
        
        this->videoPlayerObject->setPlayPosition(0);
    }
}

void IRVideoAnnotater::updateAnnotation()
{
    //updateVideoPlayerOfWorkspace();
}

// ==================================================

void IRVideoAnnotater::applyEventsOnTheLoadedVideo(VideoAnnotationEventComponent* event)
{
    if(this->myVideoPlayerObject.get() == nullptr) return;
    
    
    
    //this->myVideoPlayerObject->updateEventComponent(event);
    //this->videoPlayerObject->updateEventComponent(event);

}

void IRVideoAnnotater::createEventOnTheLoadedVideo(VideoAnnotationEventComponent* event)
{
    if(this->myVideoPlayerObject.get() == nullptr) return;
    
    
}

void IRVideoAnnotater::deleteEventOnTheLoadedVideo()
{
    if(this->myVideoPlayerObject.get() == nullptr) return;
    // stop playing video first
    //this->videoPlayerObject->stop();
    this->myVideoPlayerObject->stop();
    
    //resize to sort
    auto currentPosition = this->eventListComponent->getViewPosition();
    this->eventListComponent->resized();
    this->eventListComponent->setViewPosition(currentPosition);
    
    auto events = this->eventListComponent->getEventComponents();
        
    //first update the videoObject on the Annotater
    //this->myVideoPlayerObject->setAnnotationEvents(events);
    //this->videoPlayerObject->setAnnotationEvents(events);
    
}


void IRVideoAnnotater::updateEventsOnTheLoadedVideo()
{

}

void IRVideoAnnotater::updateAnnotationData()
{
    this->annotationData.clear();
    
    for(auto e : this->eventListComponent->getEventComponents())
    {
        float beginTime = e->getBeginTimeCode();
        float endTime = e->getEndTimeCode();
        
        this->annotationData.push_back(AnnotationChart::annotationData(beginTime, endTime));
    }
    
}

// ==================================================

void IRVideoAnnotater::nodeObjectSelectionChange(IRNodeObject* obj)
{
    std::cout << "nodeObjectSelectionChange "<< obj << " : " << obj->isSelected()<< std::endl;
    
    // return when obj is DESELECTED
    if(!obj->isSelected()) return;
    
    this->eventLogList->setLogComponent(obj->getObjController());

    VideoAnnotationEventComponent* event = static_cast<VideoAnnotationEventComponent*>(obj->getEventComponent());
    
    this->eventListComponent->deSelectAllEventComponents();
    
    std:: cout << "event select  = "<< event << std::endl;
    if(event != nullptr){
        this->eventListComponent->selectEventComponent(event);
        //this->videoTransport->setCurrentPlayingPosition(obj->getBeginTimeCode());
        
        //this->myVideoPlayerObject->setPlayPosition(obj->getBeginTimeCode());
    }
    //event->setSelected(true);
    
  
    
}
void IRVideoAnnotater::nodeObjectGetFocused(IRNodeObject* obj)
{
    
}
void IRVideoAnnotater::editModeChanged(IRWorkspaceComponent* changedSpace)
{
    
}
void IRVideoAnnotater::heavyObjectCreated(IRNodeObject* obj)
{
    
}

void IRVideoAnnotater::deselectAllObjectsOnWorkspace()
{
    //this->workspace->deselectAllObjects();
    this->myVideoPlayerObject->getWorkspace()->deselectAllObjects();
}

// ==================================================

void IRVideoAnnotater::loadAndApplySRTs()
{
    
    std::cout << "loadAndApplySRTs\n";
    std::cout << this->jsonManager.getData().dump() << std::endl;
    auto space = this->myVideoPlayerObject->getWorkspace();
    space->loadAndApplyIRSRT(this->jsonManager.getData());
    
    // disable update annotation data to avoid operating the task repeatedly
    this->enableUpdateAnnotationData = false;
    for(auto obj : space->getObjectList())
    {
        std::cout << obj->name << std::endl;
        createEventComponent(obj);
        
    }
    // enable update annotation data
    this->enableUpdateAnnotationData = true;
    
    updateAnnotationData();
    
    // reset video playing position
    this->myVideoPlayerObject->setPlayPosition(0);

    resized();

    
}


// ==================================================

// ==================================================
