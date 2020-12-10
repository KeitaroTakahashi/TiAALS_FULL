//
//  IRWorkspace.cpp
//  IRiMaS
//
//  Created by Keitaro on 18/09/2019.
//

#include "IRWorkspace2.hpp"

IRWorkspace::IRWorkspace(String title, Rectangle<int> draggableMargin, IRStr* str, int index) :
IRWorkspaceComponent(title, draggableMargin, str, index)
{
    //enableDrawGrids(true);
}

IRWorkspace::~IRWorkspace()
{
    
}

// =============================================

void IRWorkspace::onPaint(Graphics& g)
{
}


// =============================================
