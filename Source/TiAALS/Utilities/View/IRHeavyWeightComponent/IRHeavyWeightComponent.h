//
//  IROpenGLRenderer.h
//  IRiMaS
//
//  Created by Keitaro on 17/11/2019.
//

#ifndef IROpenGLRenderer_h
#define IROpenGLRenderer_h

#include "JuceHeader.h"

/*
    This class converts a light-weight juce::Component to heavy-weight Component by the power of OpenGL.
    It uses Objective-C++ to access NSOpenGLView->NSView and thus, it only supports macOS.
    Use this class by inheriting in your juce::Component based objects.
    Keitaro 2019 Nov
 */


// add glReadPixels function to "extensions" of juce::OpenGLContext
/*
#define IR_GL_PIXEL_FUNCTIONS(USE_FUNCTION) \
    USE_FUNCTION (glReadPixels,     void, (GLint p1, GLint p2, GLsizei p3, GLsizei p4, GLenum p5, GLenum p6, void * p7), (p1, p2, p3, p4, p5, p6, p7))
 */
// open juce_OpenGLExtensions.h and rewrite following place
/*
 #define JUCE_GL_EXTENSION_FUNCTIONS(USE_FUNCTION) \
 USE_FUNCTION (glIsRenderbuffer,         GLboolean, (GLuint p1), (p1))\
 USE_FUNCTION (glBindRenderbuffer,       void, (GLenum p1, GLuint p2), (p1, p2))\
 USE_FUNCTION (glDeleteRenderbuffers,    void, (GLsizei p1, const GLuint* p2), (p1, p2))\
 USE_FUNCTION (glGenRenderbuffers,       void, (GLsizei p1, GLuint* p2), (p1, p2))\
 USE_FUNCTION (glRenderbufferStorage,    void, (GLenum p1, GLenum p2, GLsizei p3, GLsizei p4), (p1, p2, p3, p4))\
 USE_FUNCTION (glGetRenderbufferParameteriv,  void, (GLenum p1, GLenum p2, GLint* p3), (p1, p2, p3))\
 USE_FUNCTION (glIsFramebuffer,          GLboolean, (GLuint p1), (p1))\
 USE_FUNCTION (glBindFramebuffer,        void, (GLenum p1, GLuint p2), (p1, p2))\
 USE_FUNCTION (glDeleteFramebuffers,     void, (GLsizei p1, const GLuint* p2), (p1, p2))\
 USE_FUNCTION (glGenFramebuffers,        void, (GLsizei p1, GLuint* p2), (p1, p2))\
 USE_FUNCTION (glCheckFramebufferStatus, GLenum, (GLenum p1), (p1))\
 USE_FUNCTION (glFramebufferTexture2D,   void, (GLenum p1, GLenum p2, GLenum p3, GLuint p4, GLint p5), (p1, p2, p3, p4, p5))\
 USE_FUNCTION (glFramebufferRenderbuffer,  void, (GLenum p1, GLenum p2, GLenum p3, GLuint p4), (p1, p2, p3, p4))\
 USE_FUNCTION (glGetFramebufferAttachmentParameteriv, void, (GLenum p1, GLenum p2, GLenum p3, GLint* p4), (p1, p2, p3, p4))\
 USE_FUNCTION (glReadPixels,     void, (GLint p1, GLint p2, GLsizei p3, GLsizei p4, GLenum p5, GLenum p6, void * p7), (p1, p2, p3, p4, p5, p6, p7))
 */


class IRHeavyWeightComponent : OpenGLRenderer
{
public:
    IRHeavyWeightComponent(juce::Component* component, String name);
    virtual ~IRHeavyWeightComponent();
    
   //==================================================
    virtual void componentCreated() {};
    //==================================================

    void setHeavyWeightBackgroundAlpha(float alpha) { this->alpha = alpha; }
    
    //==================================================
    //OpenGL
    OpenGLContext ctx;
    void bringThisToFront(String flag = "") {
        
        this->name = flag;
        bringToFrontObj(this->component);
        this->component->toFront(true);
        //callback
        bringToFrontCompleted();
    }
    
    void heavyWeightRepaint(String flag = "")
    {
        
        //std::cout << flag << std::endl;
        this->ctx.triggerRepaint();
    }
    
    //==================================================
    
    virtual void bringToFrontCompleted() {}
    
    juce::Component* component;
    
private:
    
    // for background transparency
    float alpha = 0.0;
    String name;
    //==================================================

    // using Objective-C++ macOS only!!
    // set up alpha value of NSOpenGLView to make transparent background
    void setComponentAlpha(GLint aValue = 0);
    // bring the juce::Component inheriting this class to the foreground of any other juce::Component or heavy-weight Components
    void bringToFrontObj(juce::Component* owner);
   
    void bringToFrontRelativeTo(IRHeavyWeightComponent* front, IRHeavyWeightComponent* back);
    
    bool exportNSImageToPNGFile(void* nsImage, int id);
    void converNSImageToJUCEImage(void* nsImage, Image* juceImage);
    
    //==================================================
    // this is the way how to render the NSView
    void* capturedData = nullptr;
    bool isCapturedFlag = false;
    void captureNSView(void* nsView);
    void convertCapturedNSViewToJuceImage(void* nsImage);
    
public:
    //==================================================

    // SCREENSHOT by Cocoa + Objective-C
    Image capturedImage;
    Image getCapturedImage() const { return this->capturedImage; }
    bool isCaptured() const { return this->isCapturedFlag; }
    
    // @pram1 WorkspaceID to distinguish the screen shot
    // Take a screenshot of only the area of workspace which excludes title  and left bars
    bool exportScreenshot(int index);
    
    // in case the component calling this method has not been binded on its parent yet,
    // then give the target Component otherwise non-binded component has null Peer!
    bool exportScreenshot(int index, juce::Component* target);
    
    void exportOpenGLPixels(int index, juce::Component* target);

    float captureImageScale = 0.25;
    
    bool isTakingScreenshot = false;
    
    std::function<void()> exportScreenshotCompleted;
    

    //==================================================
private:
    void newOpenGLContextCreated()override
    {
        setComponentAlpha(this->alpha);
        componentCreated();
    }
    
    virtual void renderOpenGL()override
    {
        // transparent background
        OpenGLHelpers::clear(Colours::transparentBlack);

    }
    
    virtual void openGLContextClosing() override {}
    
    // windowID of NSWindow
    uint32 windowID = 0;
    

    bool exportOpenGLPixel = false;
    
    CriticalSection lock;
    
    void renderOpenGLContextToImage()
    {
        int x = this->component->getX();
        int y = this->component->getY();
        int w = this->component->getWidth();
        int h = this->component->getHeight();
        
        if(w > 0 && h > 0)
        {
            unsigned char* pixels = new unsigned char[4 * w * h];

            this->ctx.extensions.glReadPixels(GLint(0), GLint(0),
                         GLsizei(w), GLsizei(h),
                         GL_RGBA, GL_UNSIGNED_BYTE,
                         pixels);
            
           
            FileOutputStream stream (File (String("/Users/keitaro/Desktop/test_") + String(rand()) + String(".png")));
            PNGImageFormat pngExporter;
            
            Image myImage (Image::PixelFormat::RGB, w, h, true);
            Image::BitmapData imageData (myImage, Image::BitmapData::readWrite);
            memcpy (imageData.data, pixels, 4 * w * h);
            
            //this->img = myImage;
            pngExporter.writeImageToStream(myImage, stream);
          
            delete [] pixels;

        }
            
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IRHeavyWeightComponent)

};

#endif /* IROpenGLRenderer_h */
