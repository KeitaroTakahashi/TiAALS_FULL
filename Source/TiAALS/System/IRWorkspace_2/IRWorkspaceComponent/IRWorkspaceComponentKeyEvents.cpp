//
//  IRWorkspaceComponentKeyEvents.cpp
//  IRiMaS
//
//  Created by Keitaro on 18/09/2019.
//

#include "IRWorkspaceComponent.hpp"

//bool IRWorkspaceComponent::keyPressed (const KeyPress& key)
bool IRWorkspaceComponent::keyPressed (const KeyPress& key, Component* originatingComponent)
{
    // for some reason, the workspace bihind the topworkspace doesnt give up receiving keyboard event...
    
    std::cout << "IRWorkspaceComponent " << this << " of " << getIndex() << " : keyPressed() of " << this->name << " : " << key.getKeyCode() << " : " << key.getTextDescription() << ", " << key.getTextCharacter() <<   std::endl;
    
  // delete key
    if(key.getKeyCode() == key.deleteKey || key.getKeyCode() == key.backspaceKey)
    {
        if(isEditMode())
            deleteSelectedObjects();
        return true;
    }
    
    if(key.getTextDescription() == "command + C")
    {
        if(this->isEditMode())
        {
            copySelectedObjects();
        }
        return true;
    }
    
    if(key.getTextDescription() == "command + V")
    {
        if(this->isEditMode())
        {
            pasteSelectedObjects();
        }
        return true;
    }
    
    if(key.getTextDescription() == "command + D")
    {
        std::cout << "duplicate\n";
        
        std::cout << "selected num = " << this->selector->getSelectedObjectList().size() << std::endl;
        
        if(this->isEditMode())
        {
            std::cout << "duplicate 2\n";

            duplicateSelectedObjects();
        }
        return true;
    }

    return false;
}



// ==================================================


void IRWorkspaceComponent::registerKeyListener(KeyListener* key)
{
    addKeyListener(key);
}
void IRWorkspaceComponent::registerMouseListener(MouseListener* mouse)
{
    addMouseListener(mouse, false);
}
