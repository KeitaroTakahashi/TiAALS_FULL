//
//  IRYoutubeObject.h
//  IRIMAS2020July
//
//  Created by Keitaro on 20/11/2020.
//

#ifndef IRYoutubeObject_h
#define IRYoutubeObject_h


#include "IRNodeObject.hpp"
#include "IRYoutube.h"

class IRYoutubeObject : public IRNodeObject
{
public:
    
    IRYoutubeObject(Component* parent, IRStr* str) :
    IRNodeObject(parent, "IRYoutubeObject", str, NodeObjectType(ordinaryIRComponent))
    {
        
        this->ui.reset( new IRYoutube(this, str) );
        //this->textEditor->addMouseListener(this, true);
        

        addAndMakeVisible(this->ui.get());
        childComponentManager(this->ui.get());
        //this->textEditor->toBack();

        setObjectSize(400, 300);
        
    }
    ~IRYoutubeObject()
    {
        this->ui.reset();
    }
    
    // --------------------------------------------------

    IRNodeObject* copyThisObject() override
    {
        return new IRYoutubeObject(this->parent, getStr());
    }
    // --------------------------------------------------
    IRNodeObject* copyContents(IRNodeObject* object) override
    {
        IRYoutubeObject* obj = static_cast<IRYoutubeObject*>(object);
        return obj;
    }
    // --------------------------------------------------
    IRNodeObject* copyDragDropContents(IRNodeObject* object) override
    {
        IRYoutubeObject* obj = new IRYoutubeObject(this->parent, getStr());
        return obj;
    }
    // --------------------------------------------------
    
    t_json saveThisToSaveData() override
    {
        t_json save = t_json::object({
            {"empty", 10}
        });
        
        return save;

    }
    void loadThisFromSaveData(t_json data) override
    {
        
    }
    

    void moveToFrontAction() override
    {
        this->ui->bringViewToFront();
    }
    
private:
 
    void resized() override
    {
        this->ui->setBounds(getLocalBounds().reduced(10));
    }
    
    void workspaceActiveStatusChanged(bool flag) override
    {
     
        if(flag) {
            addAndMakeVisible(this->ui.get());
        }else{
            removeChildComponent(this->ui.get());
        }
    }
    
    std::shared_ptr<IRYoutube> ui;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IRYoutubeObject)

};
#endif /* IRYoutubeObject_h */
