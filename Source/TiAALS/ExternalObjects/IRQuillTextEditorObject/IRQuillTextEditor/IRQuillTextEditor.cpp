//
//  IRQuillTextEditor.cpp
//  IRiMaS
//
//  Created by Keitaro on 21/05/2020.
//

#include "IRQuillTextEditor.hpp"


IRQuillTextEditor::IRQuillTextEditor(IRNodeObject* parent, IRStr* str) :
IRUIFoundation(parent, str)
{
    
    //addAndMakeVisible(this->b);
    addAndMakeVisible(&this->browser);
    
#if JUCE_MAC
    this->url = "/Contents/Resources/materials/Sources/WEB/Quill/quill.html";
#elif JUCE_IOS
    this->url = "/materials/Sources/WEB/Quill/quill.html";
#endif
    
    this->url = "file://" + File::getSpecialLocation(File::currentApplicationFile).getFullPathName() + this->url;
    
    std::cout << "IRQuillTextEditor : " << this->url << std::endl;

    this->browser.getBrowser()->goToURL(this->url);
}

IRQuillTextEditor::~IRQuillTextEditor()
{

}


void IRQuillTextEditor::resized()
{
    this->browser.setBounds(getLocalBounds());
    
    //b.setBounds(0, 0, 100, 100);
}

// ==================================================
void IRQuillTextEditor::bringViewToFront()
{
    toFront(true);
    this->browser.bringViewToFront();
}



// ==================================================

void IRQuillTextEditor::mouseDown(const MouseEvent& e)
{
    std::cout << "IRQuillTextEditor::mouseDown\n";
}
