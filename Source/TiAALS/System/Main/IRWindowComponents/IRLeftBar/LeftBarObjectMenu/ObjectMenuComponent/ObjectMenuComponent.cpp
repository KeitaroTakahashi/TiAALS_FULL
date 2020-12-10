//
//  ObjectMenuComponent.cpp
//  IRiMaS
//
//  Created by Keitaro on 11/08/2019.
//

#include "ObjectMenuComponent.hpp"

ObjectMenuComponent::ObjectMenuComponent(IRStr* str, Rectangle<int> frameRect) :
IRStrComponent(str),
IRHeavyWeightComponent(this, "ObjectMenuComponent"),
frameRect(frameRect)
{
    this->title.setColour(Label::textColourId, getStr()->SYSTEMCOLOUR.text);
    this->title.setJustificationType(Justification::centred);
    addAndMakeVisible(&this->title);
    
}

ObjectMenuComponent::~ObjectMenuComponent()
{
    for(auto item : this->items)
    {
        delete item;
    }
    this->items.clear();
}

// ==================================================
void ObjectMenuComponent::paint(Graphics& g)
{
    g.fillAll(getStr()->SYSTEMCOLOUR.contents);
    
    
}

void ObjectMenuComponent::resized()
{
    int y = 10;
    int x = getWidth() / 2;
    this->title.setBounds(0, y, 100, 30);
    //this->title.setCentrePosition(x, y);
    y += 20;
    for(auto item : this->items)
    {
        int s = this->buttonSize + yMarge*2;
        y += s;
        item->setSize(getWidth(), s);
        item->setCentrePosition(x, y);
    }
}

// ==================================================

// ==================================================
void ObjectMenuComponent::setTitleText(String text)
{
    this->title.setText(text, dontSendNotification);
}
// ==================================================

void ObjectMenuComponent::createItem(IRObjectFactory2::t_object* obj)
{
    this->objects.push_back(obj);
    this->items.push_back(new ObjectMenuItem(this, obj, this->buttonSize));
    
    addAndMakeVisible(this->items[this->items.size() - 1]);
    
}
// ==================================================

void ObjectMenuComponent::ObjectMenuItemClicked(ObjectMenuItem* item)
{
    //deselect
    for(auto i : this->items)
    {
        if(i != item) i->setSelected(false);
    }
    
    //virtual function
    itemSelected(item->getObject());
}

void ObjectMenuComponent::ObjectMenuItemMouseUp(ObjectMenuItem* item)
{
    itemReleased(item->getObject());
    
}

void ObjectMenuComponent::itemReleased(IRObjectFactory2::t_object* obj)
{
    IRObjectFactory2& factory = singleton<IRObjectFactory2>::get_instance();

    auto topSpace = static_cast<IRWorkspace* >(getStr()->TopWorkspace);
    
    auto nodeObj = factory.createObject(obj->id, topSpace, getStr());
    nodeObj->setObjectCentredPosition(topSpace->getWidth()/2,
                                      topSpace->getHeight()/2);
    topSpace->createObject(nodeObj);
    
}
// ==================================================
