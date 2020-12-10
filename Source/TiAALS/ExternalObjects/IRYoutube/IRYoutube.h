//
//  IRYoutube.h
//  IRIMAS2020July
//
//  Created by Keitaro on 20/11/2020.
//

#ifndef IRYoutube_h
#define IRYoutube_h


#include "IRUIFoundation.hpp"
#include "IRWebComponent.h"

class IRYoutube : public IRUIFoundation
{
public:
    IRYoutube(IRNodeObject* parent, IRStr* str) :
    IRUIFoundation(parent, str)
    {
        
        //addAndMakeVisible(this->b);
        addAndMakeVisible(&this->browser);
        
        #if JUCE_MAC
            this->url = "/Contents/Resources/materials/Sources/WEB/Youtube/index.html";
        #elif JUCE_IOS
            this->url = "/materials/Sources/WEB/Youtube/index.html";
        #endif
        
        this->url = "file://" + File::getSpecialLocation(File::currentApplicationFile).getFullPathName() + url;
        

        //this->url = "https://youtu.be/EHs7Av5TZhQ";

        this->browser.getBrowser()->goToURL(this->url);
    }
    ~IRYoutube() {}
    
    // ==================================================

    void resized() override
    {
        this->browser.setBounds(getLocalBounds());
    }
    // ==================================================

    void bringViewToFront()
    {
        toFront(true);
        this->browser.bringViewToFront();
    }
    // ==================================================
    // ==================================================

private:
    // ==================================================
    void mouseDown(const MouseEvent& e) override
    {
        
    }
    
    // ==================================================
    IRWebComponent browser;
    
    String url;
    
    //TextButton b;

    // ==================================================
    // ==================================================

};

#endif /* IRYoutube_h */
