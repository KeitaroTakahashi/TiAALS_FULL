//
//  ObjectsMenuComponent.cpp
//  IRiMaS
//
//  Created by Keitaro on 27/08/2019.
//

#include "ObjectsMenuComponent.hpp"

ObjectsMenuComponent::ObjectsMenuComponent(IRStr* str, Rectangle<int> frameRect) : ObjectMenuComponent(str, frameRect)
{
    setTitleText("Objects");
    IRObjectFactory2& factory = singleton<IRObjectFactory2>::get_instance();
    auto objects = factory.getRegisteredObjectOfCategory(objectCategory::OBJECTMENU);

    for(auto o : objects)
    {
        std::cout << o->name << std::endl;
        createItem(o);
    }
}

ObjectsMenuComponent::~ObjectsMenuComponent()
{
    
}

void ObjectsMenuComponent::paint(Graphics& g)
{
    g.fillAll(getStr()->SYSTEMCOLOUR.contents);
}

void ObjectsMenuComponent::resized()
{
    ObjectMenuComponent::resized();
}
// ==================================================

void ObjectsMenuComponent::itemSelected(IRObjectFactory2::t_object* obj)
{
    
}

void ObjectsMenuComponent::itemReleased(IRObjectFactory2::t_object* obj)
{
    IRObjectFactory2& factory = singleton<IRObjectFactory2>::get_instance();

    auto topSpace = static_cast<IRWorkspace* >(getStr()->TopWorkspace);
    
    auto nodeObj = factory.createObject(obj->id, topSpace, getStr());
    nodeObj->setObjectCentredPosition(topSpace->getWidth()/2,
                                      topSpace->getHeight()/2);
    topSpace->createObject(nodeObj);
}

// ==================================================

