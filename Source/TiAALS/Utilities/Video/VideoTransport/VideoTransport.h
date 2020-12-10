//
//  VideoTransport.h
//  IRiMaS
//
//  Created by Keitaro on 08/12/2019.
//

#ifndef VideoTransport_h
#define VideoTransport_h

#include "JuceHeader.h"
#include "IRImageButton.hpp"
#include "TimeCode.h"
#include "VideoSegmentTimeSlider.h"
#include "IRVideoAnnotaterDelegate.h"

class IRVideoTransport : public Component,
public IRStrComponent,
public IRVideoAnnotaterDelegate,
public ChangeBroadcaster,
private Slider::Listener
{
public:
    
    enum IRVideoTransportStatus
    {
        OpenVideoFile,
        OpenAnnotationFile,
        SaveAnnotationFile,
        addEventButtonClicked,
        deleteEventButtonClicked,
        play,
        stop,
        playPositionChanged,
        videoSegmentChanged,
        videoSegmentMinChanged,
        videoSegmentMaxChanged,
        NONE
    };
    
    IRVideoTransport(IRStr* str, IRVideoAnnotaterBase* base) :
    IRStrComponent(str),
    IRVideoAnnotaterDelegate(base),
    timeCode(str, 0),
    videoSegmentSlider(str, 0)
    {
        addAndMakeVisible(&this->openVideoButton);
        this->openVideoButton.onClick = [this]{ openVideoButtonClicked(); };
        this->openVideoButton.setButtonText("Open Video");
        
        /*
        addAndMakeVisible(&this->openAnnotationButton);
        this->openAnnotationButton.onClick = [this]{ openAnnotationButtonClicked(); };
        this->openAnnotationButton.setButtonText("Open IRSRT");
        
        addAndMakeVisible(&this->saveAnnotationButton);
        this->saveAnnotationButton.onClick = [this]{ saveAnnotationButtonClicked(); };
        this->saveAnnotationButton.setButtonText("Save IRSRT");
        */
        createButton(&this->addEventButton, getStr()->ICONBANK->icon_newSlide);
        this->addEventButton.setDrawCircle(false);
        this->addEventButton.setDrawRoundedSquare(true);
        this->addEventButton.onClick = [this]{ addEventButtonClickedAction(); };
        
        createButton(&this->deleteEventButton, getStr()->ICONBANK->icon_deleteSlide);
        this->deleteEventButton.setDrawCircle(false);
        this->deleteEventButton.setDrawRoundedSquare(true);
        this->deleteEventButton.onClick = [this]{ deleteEventButtonClickedAction(); };


        //controller
        createButton(&this->playButton, getStr()->ICONBANK->icon_play);
        this->playButton.onClick = [this]{ playButtonClickedAction(); };
        createButton(&this->stopButton, getStr()->ICONBANK->icon_stop);
        this->stopButton.onClick = [this]{ stopButtonClickedAction(); };
        
        addAndMakeVisible(this->timeCode);
        this->timeCode.timeCodeChangedCallback = [this]{timeCodeChanged();};
        
        this->timeCodeSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
        this->timeCodeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox,
                                                true, 0, 0);
        addAndMakeVisible(this->timeCodeSlider);
        this->timeCodeSlider.addListener(this);
        
        
        addAndMakeVisible(this->videoSegmentSlider);
        //this->videoSegmentSlider.videoSegmentChangedCallback = [this]{ videoSegmentChangedAction(); };
        this->videoSegmentSlider.videoSegmentMinChangedCallback = [this] { videoSegmentMinChangedAction(); };
        this->videoSegmentSlider.videoSegmentMaxChangedCallback = [this] { videoSegmentMaxChangedAction(); };

    }
    
    ~IRVideoTransport()
    {
        
    }
    
    // ==================================================
    
    virtual void paint(Graphics& g) override
    {
        g.fillAll(getStr()->SYSTEMCOLOUR.contents);
        g.setColour(getStr()->SYSTEMCOLOUR.fundamental);
        
        /*
        g.drawLine(this->playButton.getX() - 5,
                   0,
                   this->playButton.getX() - 5,
                   getHeight(),
                   2);
        
        g.drawLine(this->timeCode.getX() - 5,
                   0,
                   this->timeCode.getX() - 5,
                   getHeight(),
                   2);
         */
        
        g.drawLine(0, getHeight() / 2,
                   getWidth(), getHeight() / 2);
    }
    
    virtual void resized() override
    {
        int h = 30;
        
        this->openVideoButton.setBounds(getWidth() - 100, 5, 90, 30);
        //this->openAnnotationButton.setBounds(getWidth() - 200, 5, 90, 30);
        //this->saveAnnotationButton.setBounds(getWidth() - 300, 5, 90, 30);
        
        juce::Point<int> b = juce::Point<int> (((float)h / this->addEventButton.getAspectRatio()), h);
        int marginX = 5;
        int x = marginX;
        
        this->videoSegmentSlider.setBounds(x, b.getY() + 5, getWidth(), 40);
        
        this->addEventButton.setBounds(x, 5, b.getX(), b.getY());
        x += marginX + b.getX();
        this->deleteEventButton.setBounds(x, 5, b.getX(), b.getY());
        
        float ratio = this->playButton.getAspectRatio();
        int buttonWidth = 30;
        int buttonHeight = buttonWidth * ratio;
        
        x += (marginX * 2) + b.getX(); // one more margin
        this->playButton.setBounds(x, 5, buttonWidth, buttonHeight);
        x += marginX + buttonWidth;
        this->stopButton.setBounds(x, 5, buttonWidth, buttonHeight);
        
        x += (marginX * 2) + buttonWidth; // one more margin
        this->timeCode.setBounds(x, 0, 110, getHeight());
        
        x += (marginX) + this->timeCode.getWidth();
        this->timeCodeSlider.setBounds(x, 0, getWidth() - x - 110, 30);
        
        

    }
    
    // ==================================================
    
    void addEventButtonClickedAction()
    {
        this->status = addEventButtonClicked;
        sendChangeMessage();
    }
    
    void deleteEventButtonClickedAction()
    {
        this->status = deleteEventButtonClicked;
        sendChangeMessage();
    }
    // ==================================================

    void openVideoButtonClicked()
    {
        this->status = OpenVideoFile;
        sendChangeMessage();
    }
    
    void openAnnotationButtonClicked()
    {
        this->status = OpenAnnotationFile;
        sendChangeMessage();
    }
    
    void saveAnnotationButtonClicked()
    {
        this->status = SaveAnnotationFile;
        sendChangeMessage();
    }
    // ==================================================
    
    void closeEventMenu()
    {
    }
    // ==================================================
    
    void setVideoLengthInSec(float videoLengthInSec)
    {
        
        std::cout << "setVideoLengthInSec : " << videoLengthInSec << std::endl;
        this->videoLengthInSec = videoLengthInSec;
        
        this->timeCode.setVideoLengthInSec(videoLengthInSec);
        
      
        if(videoLengthInSec > 0)
        {
            std::cout<< "videoLengthInSec = " << videoLengthInSec << std::endl;
            this->timeCodeSlider.setRange(0.0, (double)videoLengthInSec, 0.1);
            this->videoSegmentSlider.setVideoLengthInSecond((double)videoLengthInSec);
            //this->timeCodeSlider.setValue(0.0);

        }
        
        std::cout << "slider max = "<< this->timeCodeSlider.getMaximum() << std::endl;
    }
    
    void setCurrentPlayingPosition(double pos)
    {
        this->timeCodeSlider.setValue(pos, dontSendNotification);
        this->timeCode.setLabelVal(pos);
    }
    
    double getCurrentPlayingPosition() const { return this->timeCodeSlider.getValue(); }
    
    void setVideoRange(float begin, float end)
    {
        if(begin > 0 && begin >= end)
        {
            begin = end - 0.1;
        }
        this->timeCodeSlider.setRange(begin, end);
        this->videoSegmentSlider.setSliderValue(begin, end);
        
        std::cout << "setVideoRange : " << begin << ", " << end << std::endl;
    }

    // ==================================================
    IRVideoTransportStatus getStatus() const { return this->status; }
    float getPlayPosition() { return this->timeCode.getTimeCodeInSec(); }
    // ==================================================
    
    
private:
    // ==================================================
    IRVideoTransportStatus status = NONE;
    float videoLengthInSec = 0;
    // ==================================================
    void createButton(IRImageButton* button, IRIconBank::IRIconImage img)
    {
        if(getStr()->SYSTEMCOLOUR.isWhiteBased)
            button->setImage(img.black);
        else  button->setImage(img.white);
        addAndMakeVisible(button);
    }
    // ==================================================

    TextButton openVideoButton;
    TextButton openAnnotationButton;
    TextButton saveAnnotationButton;
    IRImageButton addEventButton;
    IRImageButton deleteEventButton;

    //controller
    IRImageButton playButton;
    void playButtonClickedAction()
    {
        this->status = play;
        sendChangeMessage();
    }
    IRImageButton stopButton;
    void stopButtonClickedAction()
    {
        this->status = stop;
        sendChangeMessage();
    }
    
    

    // ==================================================
    TimeCode timeCode;
    void timeCodeChanged()
    {
        float timeInSec = this->timeCode.getTimeCodeInSec();
        /*
        std::cout << "timeInSec = " << timeInSec << " of " << this->timeCode.getVideoLengthInSec() << " : " << this->timeCodeSlider.getMaxValue() << std::endl;*/
        if(timeInSec > this->videoLengthInSec)
        {
            timeInSec = this->videoLengthInSec;
        }
        this->timeCodeSlider.setValue(timeInSec, dontSendNotification);
        
        this->status = playPositionChanged;
        sendChangeMessage();

    }
    // ==================================================
    
    Slider timeCodeSlider;
    
    void sliderValueChanged (Slider *slider) override
    {
        if(slider == &this->timeCodeSlider)
        {
            
            float val = slider->getValue();
            this->timeCode.setLabelVal(val);
            this->status = playPositionChanged;
            sendChangeMessage();
        }
    }
    // ==================================================
    
    VideoSegmentTimeSlider videoSegmentSlider;

    void videoSegmentChangedAction()
    {
        this->status = videoSegmentChanged;
        sendChangeMessage();
    }
    
    void videoSegmentMinChangedAction()
    {
        std::cout << "videoSegmentMinChangedAction\n";
        
        float begin = this->videoSegmentSlider.getSegmentValue()[0];
        float end = this->videoSegmentSlider.getSegmentValue()[1];
        setVideoRange(begin, end);
        setCurrentPlayingPosition(begin);
        /*
        this->status = videoSegmentMinChanged;
        sendChangeMessage();
           */
        // callback to sync : note! sendChangeMessage() does not sync
        if(this->videoSegmentMinChangedCallback != nullptr)
            this->videoSegmentMinChangedCallback();
       
    }
    
    void videoSegmentMaxChangedAction()
    {
        std::cout << "videoSegmentMaxChangedAction\n";

        float begin = this->videoSegmentSlider.getSegmentValue()[0];
        float end = this->videoSegmentSlider.getSegmentValue()[1];
        setVideoRange(begin, end);
        setCurrentPlayingPosition(end);
        
        /*
        this->status = videoSegmentMaxChanged;
        sendChangeMessage();
         
           */
        // callback to sync : note! sendChangeMessage() does not sync
        if(this->videoSegmentMaxChangedCallback != nullptr)
            this->videoSegmentMaxChangedCallback();
      
    }
    
public:
    // use the following functions when you want to sync any methods called when min or max value of video segments are modified.
    std::function<void()> videoSegmentMinChangedCallback;
    std::function<void()> videoSegmentMaxChangedCallback;
    
    std::vector<float> getSegmentValue()
    {
        return this->videoSegmentSlider.getSegmentValue();
    }
    
    // ==================================================
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IRVideoTransport)

};

#endif /* VideoTransport_h */
