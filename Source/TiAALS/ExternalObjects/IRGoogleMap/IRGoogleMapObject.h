//
//  IRGoogleMapObject.h
//  IRIMAS2020July
//
//  Created by Keitaro on 20/11/2020.
//

#ifndef IRGoogleMapObject_h
#define IRGoogleMapObject_h


#include "IRNodeObject.hpp"
#include "IRGoogleMap.h"

class IRGoogleMapObject : public IRNodeObject
{
public:
    
    IRGoogleMapObject(Component* parent, IRStr* str) :
    IRNodeObject(parent, "IRGoogleMapObject", str, NodeObjectType(ordinaryIRComponent))
    {
        
        this->ui.reset( new IRGoogleMap(this, str) );
        //this->textEditor->addMouseListener(this, true);
        

        addAndMakeVisible(this->ui.get());
        childComponentManager(this->ui.get());
        //this->textEditor->toBack();

        setObjectSize(400, 300);
        
    }
    ~IRGoogleMapObject()
    {
        this->ui.reset();
    }
    
    // --------------------------------------------------

    IRNodeObject* copyThisObject() override
    {
        return new IRGoogleMapObject(this->parent, getStr());
    }
    // --------------------------------------------------
    IRNodeObject* copyContents(IRNodeObject* object) override
    {
        IRGoogleMapObject* obj = static_cast<IRGoogleMapObject*>(object);
        return obj;
    }
    // --------------------------------------------------
    IRNodeObject* copyDragDropContents(IRNodeObject* object) override
    {
        IRGoogleMapObject* obj = new IRGoogleMapObject(this->parent, getStr());
        return obj;
    }
    // --------------------------------------------------
    

    void moveToFrontAction() override
    {
        this->ui->bringViewToFront();
    }
    
private:
 
    void resized()
    {
        this->ui->setBounds(getLocalBounds().reduced(10));
    }
    
    std::shared_ptr<IRGoogleMap> ui;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IRGoogleMapObject)

};

#endif /* IRGoogleMapObject_h */
