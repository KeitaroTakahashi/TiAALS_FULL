//
//  IRVideoAnnotaterInterfaceObject.cpp
//  IRiMaS
//
//  Created by Keitaro on 19/01/2020.
//

#include "IRVideoAnnotaterInterfaceObject.hpp"

IRVideoAnnotaterInterfaceObject::IRVideoAnnotaterInterfaceObject(Component* parent, IRStr* str) :
IRVideoAnnotaterObject2 (parent, str)
{

    std::cout << " ========== IRVideoAnnotaterInterfaceObject creating.... ==========\n";
    this->controller.reset( new IRVideoPlayerController(str, this) );
    this->controller->addChangeListener(this);
    this->controller->getArrangeController()->addChangeListener(this);
    setObjController(this->controller.get());
    
    // disable selector on this VideoAnnotator BUT keep enabling selector for Annotator window.
    getWorkspace()->enableSelector(false);
}

IRVideoAnnotaterInterfaceObject::~IRVideoAnnotaterInterfaceObject()
{
    this->controller.reset();
}

void IRVideoAnnotaterInterfaceObject::onResized()
{
    
    //std::cout << "IRVideoAnnotaterInterfaceObject::resized\n";
    IRVideoAnnotaterObject2::onResized();
    resizeAndCentredThisComponent(getBounds());
    
    this->controller->getArrangeController()->setRectangle(getBounds());

}


// --------------------------------------------------

IRNodeObject* IRVideoAnnotaterInterfaceObject::copyThisObject()
{
    
    std::cout << "IRVideoAnnotaterInterfaceObject::copyThis\n";
    auto v = new IRVideoAnnotaterInterfaceObject(this->parent, getStr());
    
    //v->setObjectBounds(getBounds());
    //getWorkspace()->copyAllDataToWorkspace(v->getWorkspace());
    
    
        
    return v;
}
// --------------------------------------------------
IRNodeObject* IRVideoAnnotaterInterfaceObject::copyContents(IRNodeObject* object)
{
    IRVideoAnnotaterInterfaceObject* obj = static_cast<IRVideoAnnotaterInterfaceObject*>(object);
    obj->setObjectBounds(getLocalBounds());
    File movieFile = getVideoPlayerObject()->getVideoPlayer()->getMovieFile();
    if(movieFile.exists())
    {
        obj->openFile(movieFile, false);
    }
    
    return obj;
}
// --------------------------------------------------
IRNodeObject* IRVideoAnnotaterInterfaceObject::copyDragDropContents(IRNodeObject* object)
{
    IRVideoAnnotaterInterfaceObject* obj = new IRVideoAnnotaterInterfaceObject(this->parent, getStr());
    return obj;
}
// --------------------------------------------------
// --------------------------------------------------
// SAVE LOAD
t_json IRVideoAnnotaterInterfaceObject::saveThisToSaveData()
{
    using t = VideoAnnotationEventComponent::VideoAnnotationType;
   
    IRVideoAnnotater* videoAnnotator = this->controller->getVideoAnnotaterComponent();
    
    std::string srtFilePath = videoAnnotator->getIRSTRFilePath().toStdString();
    auto srtSaveData = videoAnnotator->getSRTSaveData();
    
    auto videoFilePath = this->getVideoPlayerObject()->getVideoPlayer()->getPath();
    
    //float segmentBegin = this->getPlayerController()->getBeginRange();
    //float segmentEnd = this->getPlayerController()->getEndRange();
    float segmentBegin = videoAnnotator->getVideoSegmentBeginTime();
    float segmentEnd = videoAnnotator->getVideoSegmentEndTime();
    
    //std::cout << "videoFilePath = " << videoFilePath << std::endl;
    //std::cout << "srtFilePath = " << srtFilePath << std::endl;
    
    std::cout << "saveThisToSaveData range : " << segmentBegin << ", " << segmentEnd << std::endl;
    
    t_json saveData = t_json::object({
        {"filePath", videoFilePath },
        //{"srtPath",  srtFilePath },
        {"srtData", srtSaveData},
        {"segment_begin", segmentBegin},
        {"segment_end", segmentEnd}
    });
   
    t_json save = t_json::object({
        {"videoPlayer", saveData}
    });
    
    return save;
}
// --------------------------------------------------
void IRVideoAnnotaterInterfaceObject::loadThisFromSaveData(t_json data)
{
    t_json w = data["videoPlayer"];
  
    File file(w["filePath"].string_value());
    //this->getVideoPlayerObject()->getVideoPlayer()->openFile(file);
    //std::string srtPath = w["srtPath"].string_value();
    auto srtSaveData = w["srtData"];

    this->begin = w["segment_begin"].number_value();
    this->end = w["segment_end"].number_value();

    IRVideoAnnotater* videoAnnotator = this->controller->getVideoAnnotaterComponent();
    videoAnnotator->setInitialVideoSegmentRange(this->begin, this->end);
    videoAnnotator->openFile(file);
    videoAnnotator->setSRTSaveData(srtSaveData);
    
    // use this flag to enable setting segment range in videoLoadCompletedCallback() method in this class
    this->saveDataLoadTrigger = true;

}
// --------------------------------------------------

void IRVideoAnnotaterInterfaceObject::videoLoadCompletedCallback()
{
    std::cout << "IRVideoAnnotaterInterfaceObject::videoLoadCompletedCallback\n";
    
    // only called when video file is loaded from this object, but not from controller.
    //this->controller->updateAnnotater();
    
    // set initial bounds of the video player to fix the annotation
    auto v = getVideoPlayerObject()->getVideoPlayer();
    setFixObjectSizeRatioWithOriginalSize(false, v->getCurrentVideoBounds().toFloat());
    
    applyAnnotation();
    
    if(this->saveDataLoadTrigger)
    {
        
        std::cout << "setVideoSegmentRange " << this->begin << ", " << this->end << std::endl;
        IRVideoAnnotater* videoAnnotator = this->controller->getVideoAnnotaterComponent();
        videoAnnotator->setVideoSegmentRange(this->begin, this->end);
        
        this->getVideoPlayerObject()->setRangeBegin(this->begin);
        this->getVideoPlayerObject()->setRangeEnd(this->end);
        this->getPlayerController()->setVideoBeginAndEndRange(this->begin, this->end);
        this->getVideoPlayerObject()->setPlayPosition(this->begin);
        
        

        this->saveDataLoadTrigger = false;
        
    }

}
// --------------------------------------------------

void IRVideoAnnotaterInterfaceObject::annotaterOpenedAction()
{
    std::cout << "annotaterOpenedAction\n";

}
// --------------------------------------------------

void IRVideoAnnotaterInterfaceObject::annotaterClosedAction()
{
    //this->parent->addAndMakeVisible(this);
    //this->setBounds(getBoundsInWorkspace());
    
    std::cout << "annotaterClosedAction\n";
}

// --------------------------------------------------

void IRVideoAnnotaterInterfaceObject::IRChangeListenerCallback (ChangeBroadcaster* source)
{
    if(source == this->controller.get())
    {
        using uiStatus = VideoController::VideoControllerStatus;
        
        switch(this->controller->getStatus())
        {
            case uiStatus::OpenVideoFile:
                getVideoPlayerObject()->openFile();
                break;
            case uiStatus::OpenVideoAnnotater:
                annotaterOpenedAction();
                break;
            case uiStatus::CloseVideoAnnotater:
                annotaterClosedAction();
                break;
            case uiStatus::ApplyAnnotation:
                applyAnnotation();
                break;
            default:
                break;
        }
    }else if(source == this->controller->getArrangeController()){
        
        using t = ArrangeController::ArrangeControllerStatus;

        switch(this->controller->getArrangeController()->getStatus())
        {
            case t::INPUTWIDTH:
                break;
            case t::INPUTHEIGHT:
                break;
            case t::INPUTX:
                break;
            case t::INPUTY:
                break;
            case t::FRONTBUTTON:
                bringToFront();
                break;
            case t::BACKBUTTON:
                bringToBack();
                break;
            default:
                break;
        }
    }
}
// --------------------------------------------------

void IRVideoAnnotaterInterfaceObject::ObjectPositionChanged(int x, int y)
{
    
    //std::cout << "IRVideoAnnotaterInterfaceObject::ObjectPositionChanged : " << x << ", " << y << std::endl;
    this->controller->getArrangeController()->setLabelX(x);
    this->controller->getArrangeController()->setLabelY(y);

}
// --------------------------------------------------

void IRVideoAnnotaterInterfaceObject::ObjectBoundsChanged(Rectangle<int> bounds)
{
    
    //std::cout << "IRVideoAnnotaterInterfaceObject::ObjectBoundsChanged\n";
}



// --------------------------------------------------
void IRVideoAnnotaterInterfaceObject::applyAnnotation()
{
    std::cout << "applyAnnotation\n";
    this->controller->updateParentVideoPlayerObject();
}
