//
//  IRQuillTextEditor.hpp
//  IRiMaS
//
//  Created by Keitaro on 21/05/2020.
//

#ifndef IRQuillTextEditor_hpp
#define IRQuillTextEditor_hpp

#include "IRUIFoundation.hpp"
#include "IRWebComponent.h"

class IRQuillTextEditor : public IRUIFoundation
{
public:
    IRQuillTextEditor(IRNodeObject* parent, IRStr* str);
    ~IRQuillTextEditor();
    
    // ==================================================

    void resized() override;
    // ==================================================

    void bringViewToFront();
    // ==================================================
    // ==================================================

private:
    // ==================================================
    void mouseDown(const MouseEvent& e) override;
    
    // ==================================================
    IRWebComponent browser;
    
    String url;
    
    //TextButton b;

    // ==================================================
    // ==================================================

};

#endif /* IRQuillTextEditor_hpp */
