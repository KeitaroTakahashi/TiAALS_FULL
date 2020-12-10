//
//  IRWorkspaceCover.cpp
//  IRiMaS
//
//  Created by Keitaro on 11/05/2020.
//

#include "IRWorkspaceCover.hpp"

IRWorkspaceCover::IRWorkspaceCover(IRStr* str) :
IRHeavyWeightComponent(this, "IRWorkspaceCover"),
IRStrComponent(str)
{
    setOpaque(false);

    openGLInit();
}

IRWorkspaceCover::~IRWorkspaceCover()
{
    std::cout << "pic w = " << this->img.getWidth() << " ; " << this->img.getHeight() <<std::endl;
    
}
// ==================================================

void IRWorkspaceCover::paint(Graphics& g)
{
    g.fillAll(this->backgroundColour);
    //g.fillAll(Colours::blue);

    if(isDrawGrids() && isEditMode()) shaderTask(g);
    
    
   

}

void IRWorkspaceCover::resized() 
{
    
}
// ==================================================
void IRWorkspaceCover::setEditMode(bool flag)
{
    this->isEditModeFlag = flag;
    repaint();
}
void IRWorkspaceCover::setBackgroundColour(Colour colour)
{
    this->backgroundColour = colour;
    repaint();
}


// ==================================================

void IRWorkspaceCover::openGLInit()
{
    
    if (auto* peer = getPeer())
            peer->setCurrentRenderingEngine (0);
                
        String url = File::getSpecialLocation(File::currentApplicationFile).getFullPathName();
    
    #if JUCE_MAC
        url += "/Contents/Resources/materials/Sources/GLSL/grid/KGrid2.frag";
    #elif JUCE_IOS
        url += "/materials/Sources/GLSL/grid/KGrid2.frag";
    #endif
        
        File f(url);
        if(!f.exists())
        {
            std::cout << "Error : " << url << " does not exist!!\n";
            AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon, "Fatal Error", "IRSpectrogram : frag file " + url + "not found! Please contact a developer with this error message.");
        }
 
        this->fragURL = url;
        this->fragmentText.load (url.toStdString());
        this->fragmentCode = fragmentText.getStdString();
        this->fragmentRefreshed = true;
    
    //std::cout << this->fragmentCode << std::endl;
    

        
}


void IRWorkspaceCover::shaderTask(Graphics& g)
{
    if (shader.get() == nullptr || shader->getFragmentShaderCode() != fragmentCode)
    {
        //shader.reset();
        if (fragmentCode.isNotEmpty() && this->fragmentRefreshed)
        {

            glEnable (GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            std::cout << "Shader Task\n";
            shader.reset (new OpenGLGraphicsContextCustomShader (fragmentCode));

            shader->onShaderActivated = [this](OpenGLShaderProgram& program){setUniform(program);};
           
            auto result = shader->checkCompilation (g.getInternalContext());
            if (result.failed()){ shader.reset();
                std::cout << "Error : shader compile error\n";
                std::cout << result.getErrorMessage() << std::endl;
            }
            this->fragmentRefreshed = false;
            
            
        }
    }
    
    if (shader.get() != nullptr)
    {
  
        shader->fillRect (g.getInternalContext(),
                          getLocalBounds()
                          );
         
    }
}

void IRWorkspaceCover::setUniform(OpenGLShaderProgram& program)
{
    
    // IMPORTANT : get the actual scale of the screen in case you are using Retina display which has double amount of piexles. Also, when there are multiple displays, then find out the current display by findDisplayForRect method.
    auto display = Desktop::getInstance().getDisplays().findDisplayForRect(getScreenBounds());
    
    auto scale = display.scale;
    int w = getWidth() * scale;
    int h = getHeight() * scale;
    
    //std::cout << "setUniform : Display Pixel size " << w << ", " << h << " : displaySize " << getWidth() << ", " << getHeight() << " : scale = " << scale << " : isMain? "<< display.isMain << " : num display = " << Desktop::getInstance().getDisplays().displays.size()<< std::endl;

    program.setUniform("resolution", w, h);
    program.setUniform("gridInterval", this->thin_grids_pixel * scale,
                       this->thick_grids_interval * this->thin_grids_pixel);
    program.setUniform("sw", (float)isEditMode());
    program.setUniform("alpha", this->gridsBackgroundAlpha);
    
    //std::cout << "setUniform " << w << ", " << h << " : scale = " << scale << " : thinGrid = " << this->thin_grids_pixel * scale <<std::endl;
   
      
}

// ==================================================

void IRWorkspaceCover::setGridValue(int thinInterval, int thickRatio)
{
    
    std::cout << "setGridValue " << thinInterval << std::endl;
    this->thin_grids_pixel = thinInterval;
    this->thick_grids_interval = thickRatio;
    repaint();
}
