//
//  IRGoogleMap.h
//  IRIMAS2020July
//
//  Created by Keitaro on 20/11/2020.
//

#ifndef IRGoogleMap_h
#define IRGoogleMap_h


#include "IRUIFoundation.hpp"
#include "IRWebComponent.h"

class IRGoogleMap : public IRUIFoundation
{
public:
    IRGoogleMap(IRNodeObject* parent, IRStr* str) :
    IRUIFoundation(parent, str)
    {
        
        //addAndMakeVisible(this->b);
        addAndMakeVisible(&this->browser);

        this->url = "https://www.google.com/maps/@38.4709385,131.3443749,5.16z";

        this->browser.getBrowser()->goToURL(this->url);
    }
    ~IRGoogleMap() {}
    
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
#endif /* IRGoogleMap_h */
