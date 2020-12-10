//
//  VAControllerComponent.h
//  IRIMAS2020July
//
//  Created by Keitaro on 11/10/2020.
//

#ifndef VAControllerComponent_h
#define VAControllerComponent_h

class VAControllerComponent : public Component,
public IRStrComponent,
public ChangeBroadcaster
{
public:
    
    enum VAControllerStatus
    {
        OpenVideoFile,
        OpenAnnotationFile,
        SaveAnnotationFile,
        OpenSRTFile,
        SaveSRTFile,
        NONE
    };
    
    // ==================================================

    
    VAControllerComponent(IRStr* str) : IRStrComponent(str)
    {
        addAndMakeVisible(&this->LabelTitle);
        this->LabelTitle.setText("Video Annotation", dontSendNotification);
        this->LabelTitle.setFont(getStr()->SYSTEMCOLOUR.h3);
        this->LabelTitle.setColour(Label::textColourId, getStr()->SYSTEMCOLOUR.text);
        this->LabelTitle.setJustificationType(Justification::left);
        
        createButtons();
    }
    
    ~VAControllerComponent()
    {
        
    }
    
private:
    // ==================================================

    void resized() override
    {
        int y = 10;
        int yIncrement = 30;
        int yBigIncrement = 40;
        
        this->LabelTitle.       setBounds(10, y, getWidth() - 20, 30);
        
        y += yBigIncrement;
        
        int menuWidth = getWidth()/2 - 20;
        
        this->openVideoButton.setBounds(10, y, menuWidth, 30);
        
        y += yBigIncrement + 10;
        this->openAnnotationButton.setBounds(10, y, menuWidth, 30);
        this->saveAnnotationButton.setBounds(20 + menuWidth, y, menuWidth, 30);
        
        y += yBigIncrement + 10;
        this->openSRTButton.setBounds(10, y, menuWidth, 30);        
        //this->saveSRTButton.setBounds(20 + menuWidth, y, menuWidth, 30);
        
    }
    
    void paint(Graphics &g) override
    {
        g.fillAll(getStr()->SYSTEMCOLOUR.contents);
        g.setColour(Colours::black);
        g.drawLine(0,42.5,getWidth(),42.5);
        
        float y = this->openAnnotationButton.getY() - 10;
        g.drawLine(0,y,getWidth(),y);

        y = this->openSRTButton.getY() - 10;
        g.drawLine(0,y,getWidth(),y);
        
    }
    // ==================================================

    void createButtons()
    {
        addAndMakeVisible(&this->openVideoButton);
        this->openVideoButton.onClick = [this]{ openVideoButtonClicked(); };
        this->openVideoButton.setButtonText("Open Video");
        
        addAndMakeVisible(&this->openAnnotationButton);
        this->openAnnotationButton.onClick = [this]{ OpenIRSRTButtonClicked(); };
        this->openAnnotationButton.setButtonText("Open IRSRT");
        
        addAndMakeVisible(&this->saveAnnotationButton);
        this->saveAnnotationButton.onClick = [this]{ SaveIRSRTButtonClicked(); };
        this->saveAnnotationButton.setButtonText("Save IRSRT");
        
        addAndMakeVisible(&this->openSRTButton);
        this->openSRTButton.onClick = [this]{ openSRTButtonClicked(); };
        this->openSRTButton.setButtonText("Open SRT");
        /*
        addAndMakeVisible(&this->saveSRTButton);
        this->saveSRTButton.onClick = [this]{ saveSRTButtonClicked(); };
        this->saveSRTButton.setButtonText("Save SRT");
         */
    }
    // ==================================================

    void openVideoButtonClicked()
    {
        this->status = OpenVideoFile;
        sendChangeMessage();
    }
    
    void OpenIRSRTButtonClicked()
    {
        this->status = OpenAnnotationFile;
        sendChangeMessage();
    }
    
    void SaveIRSRTButtonClicked()
    {
        this->status = SaveAnnotationFile;
        sendChangeMessage();
    }
    
    void openSRTButtonClicked()
    {
        this->status = OpenSRTFile;
        sendChangeMessage();
    }
    
    void saveSRTButtonClicked()
    {
        this->status = SaveSRTFile;
        sendChangeMessage();
    }
    
    // ==================================================
public:
    VAControllerStatus getStatus() const { return this->status; }
    // ==================================================
    // ==================================================

private:
    
    VAControllerStatus status = NONE;
     
    Label LabelTitle;
    
    TextButton openVideoButton;
    
    TextButton openAnnotationButton;
    TextButton saveAnnotationButton;
    
    TextButton openSRTButton;
    TextButton saveSRTButton;
};

#endif /* VAControllerComponent_h */
