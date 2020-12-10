//
//  IROpenGLRenderer.m
//  IRiMaS
//
//  Created by Keitaro on 17/11/2019.
//

#import <AppKit/AppKit.h>
#include "IRHeavyWeightComponent.cpp"

IRHeavyWeightComponent::IRHeavyWeightComponent(juce::Component* component, String name) :
component ( component ),
name(name)
{
    this->ctx.setRenderer(this);
    this->ctx.attachTo(*component);
    this->ctx.setContinuousRepainting(false);
        
}

IRHeavyWeightComponent::~IRHeavyWeightComponent()
{
    if(this->isCapturedFlag)
    {
        [(NSImage* )this->capturedData release];
        this->isCapturedFlag = false;
    }
    
    this->ctx.detach();
    std::cout << "OpenGL Context detached! : " << this->name << std::endl;
}

void IRHeavyWeightComponent::setComponentAlpha(GLint aValue)
{
    NSOpenGLContext* context = (NSOpenGLContext*)this->ctx.getRawContext();
    
    if(context != nullptr)
        [context setValues:&aValue forParameter:NSOpenGLCPSurfaceOpacity];
    else
        std::cout << "Error : IRHeavyWeightComponent::setOpenGLContextAlpha() : could not get NSOpenGLContext : nullptr in " << this->name << std::endl;
}

void IRHeavyWeightComponent::bringToFrontObj(juce::Component* owner)
{
    
    NSOpenGLContext* context = (NSOpenGLContext*)this->ctx.getRawContext();
    
    if(context == nullptr)
    {
        std::cout << "Error : IRHeavyWeightComponent::bringOpenGLContextFront() : could not get NSOpenGLContext : nullptr in " << this->name << std::endl;
        return;
    }
    
    

    auto* peer = owner->getPeer();
    if(peer != nullptr)
    {
        auto peerView = (NSView*) peer->getNativeHandle();
        [context.view retain];
        [context.view removeFromSuperview];
        [peerView addSubview: context.view];
        [context.view release];
        
        
    }
    
    [context.view setHidden: ! owner->isShowing()];
    // re-setup component alpha
    setComponentAlpha(this->alpha);

    // reset repainting 
    this->ctx.setContinuousRepainting(false);
    
    //update windowID
    this->windowID = (uint32)[[[NSApplication sharedApplication] keyWindow] windowNumber];

    this->exportOpenGLPixel = true;

    
}



void IRHeavyWeightComponent::bringToFrontRelativeTo(IRHeavyWeightComponent* front, IRHeavyWeightComponent* back)
{
    
    NSOpenGLContext* backCtx = (NSOpenGLContext*)back->ctx.getRawContext();

    if(backCtx == nullptr)
    {
        std::cout << "Error : IRHeavyWeightComponent::bringOpenGLContextFront() : could not get NSOpenGLContext : nullptr in " << back->name << std::endl;
        return;
        
    }
    
    NSOpenGLContext* frontCtx = (NSOpenGLContext*)this->ctx.getRawContext();
    
    if(frontCtx == nullptr)
    {
        std::cout << "Error : IRHeavyWeightComponent::bringOpenGLContextFront() : could not get NSOpenGLContext : nullptr in " << this->name << std::endl;
        return;
    }

    auto* f = front->component->getPeer();
    auto* b = back->component->getPeer();
    
    if(f != nullptr && b != nullptr)
    {
        auto fView = (NSView*) f->getNativeHandle();
        auto bView = (NSView*) b->getNativeHandle();
        
       //[backCtx.view addSubview:myView positioned:NSWindowBelow relativeTo:frontCtx.view];
/*
        [context.view retain];
        [context.view removeFromSuperview];
        [peerView addSubview: context.view];
        [context.view release];
 */
    }
    

}


//check further discussion at https://developer.apple.com/forums/thread/88315
void IRHeavyWeightComponent::captureNSView(void* nsView)
{
    if(this->isCapturedFlag)
    {
        [(NSImage* )this->capturedData release];
        this->isCapturedFlag = false;
    }
        
    NSView* view = (NSView*)nsView;
    NSSize mySize = view.bounds.size;
    NSSize imgSize = NSMakeSize( mySize.width, mySize.height );

    NSBitmapImageRep *bir = [view bitmapImageRepForCachingDisplayInRect:[view bounds]];
    [bir setSize:imgSize];
    [view cacheDisplayInRect:[view bounds] toBitmapImageRep:bir];

    NSImage* image = [[NSImage alloc]initWithSize:imgSize] ;
    [image addRepresentation:bir];
    
   
    this->capturedData = image;
    this->isCapturedFlag = true;
}

// check further discussion at https://forum.juce.com/t/nsimage-to-juce-image/26868/4

void IRHeavyWeightComponent::convertCapturedNSViewToJuceImage(void *nsImage)
{
    
    NSImage* img = (NSImage*) nsImage;
    if (img != nil)
    {
        [img lockFocus];
        NSBitmapImageRep* bitmapRep = [[NSBitmapImageRep alloc] initWithFocusedViewRect:NSMakeRect (0, 0, img.size.width, img.size.height)];
        [img unlockFocus];
                
        NSDictionary* dic = [NSDictionary dictionary];
        NSData* data = [bitmapRep representationUsingType:NSPNGFileType properties:dic];
        
        this->capturedImage = PNGImageFormat::loadFrom (data.bytes, data.length);
        
        //[bitmapRep release];
        //[data release];
        //std::cout << "juceImage = " << juceImage.getWidth() << ", " << juceImage.getHeight() << std::endl;

       //return {};
    }else{
    
        std::cout << "convertCapturedNSViewToJuceImage failed\n";
        //return {};
    }
}

bool IRHeavyWeightComponent::exportScreenshot(int index)
{
    //std::cout << "exportScreenshot : " << this << " : " << this->name << std::endl;


    auto owner = this->component;
    std::cout << "exportScreenshot : " << this->component << std::endl;
    auto* peer = owner->getPeer();
    if(peer == nullptr)
    {
        std::cout << "peer is null\n";
        return false;
    }
    
    std::cout << "PEER = " << peer << std::endl;
    auto peerView = (NSView*) peer->getNativeHandle();
    
    auto b = peerView.frame;
    
    // get current display and check the scale in case using Retina Display which has double resolution.
    auto display = Desktop::getInstance().getDisplays().findDisplayForRect(this->component->getScreenBounds());
    auto scale = display.scale;

    std::cout << "screenshot scale = " << scale << std::endl;
    int leftMargin = 55.0 * scale;
    int topMargin = 70.0 * scale;
    int w = (b.size.width - leftMargin) * scale;
    int h = (b.size.height - topMargin) * scale;
  
    CGRect cg = CGRectMake(leftMargin, topMargin, w, h);
    CGImageRef entireWindowShot = CGWindowListCreateImage(CGRectNull,
                                                    kCGWindowListOptionIncludingWindow,
                                                    (CGWindowID)this->windowID,
                                                    //(CGWindowID)[[[NSApplication sharedApplication] keyWindow] windowNumber], // don't do this to avoid calling UI API from the non-main Thread
                                                    kCGWindowImageBoundsIgnoreFraming);

    if(entireWindowShot == nullptr) return false;
    
    CGImageRef screenShot = CGImageCreateWithImageInRect(entireWindowShot, cg);

    CFRelease(entireWindowShot);

    // ------ scall down NSImage

    NSBitmapImageRep *imageRep = [[NSBitmapImageRep alloc] initWithCGImage:screenShot];
    NSData *tiffData = imageRep.TIFFRepresentation;
    NSImage *image = [[NSImage alloc] initWithData:tiffData];
    
    NSSize newSize = NSMakeSize(cg.size.width * this->captureImageScale,
                                cg.size.height * this->captureImageScale);
    NSImage *smallImage = [[NSImage alloc] initWithSize: newSize];

    // Report an error if the source isn't a valid image
    if (![image isValid]){
        std::cout << "Invalid Image" << std::endl;
        return false;
    } else {
        [smallImage lockFocus];
        [image setSize: newSize];
        [[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
        [image drawAtPoint:NSZeroPoint fromRect:CGRectMake(0, 0, newSize.width, newSize.height) operation:NSCompositeCopy fraction:1.0];
        [smallImage unlockFocus];
    }
    CFRelease(imageRep);
    CFRelease(image);
    CFRelease(screenShot);

    
    if(!exportNSImageToPNGFile(smallImage, index))
    {
        std::cout << "Error : exportNSImageToPNGFile failed\n";
    }
    
    CFRelease(smallImage);
    
    
    // inform the completion
    if(this->exportScreenshotCompleted != nullptr)
        this->exportScreenshotCompleted();
    
    return true;
}

bool IRHeavyWeightComponent::exportScreenshot(int index, juce::Component* target)
{
    
    
    //std::cout << "exportScreenshot : " << this << " : " << this->name << std::endl;
    exportOpenGLPixels(index, target);
    
    
    auto owner = target;
    std::cout << "exportScreenshot : " << this->component << std::endl;
    auto* peer = owner->getPeer();
    if(peer == nullptr)
    {
        std::cout << "peer is null\n";
        return false;
    }
    
    auto b = owner->getBounds();
    // get current display and check the scale in case using Retina Display which has double resolution.
    auto display = Desktop::getInstance().getDisplays().findDisplayForRect(this->component->getScreenBounds());
    auto scale = display.scale;

    std::cout << "screenshot scale = " << scale << std::endl;
    int leftMargin = 55;
    int topMargin = 70;
    int x = leftMargin * scale;
    int y = topMargin * scale;
    int w = (b.getWidth()) * scale;
    int h = (b.getHeight()) * scale;

    CGRect cg = CGRectMake(x, y, w, h);
    CGImageRef entireWindowShot = CGWindowListCreateImage(CGRectNull,
                                                    kCGWindowListOptionIncludingWindow,
                                                    (CGWindowID)this->windowID,
                                                    //(CGWindowID)[[[NSApplication sharedApplication] keyWindow] windowNumber], // don't do this to avoid calling UI API from the non-main Thread
                                                    kCGWindowImageBoundsIgnoreFraming);

    if(entireWindowShot == nullptr) return false;
    
    CGImageRef screenShot = CGImageCreateWithImageInRect(entireWindowShot, cg);

    CFRelease(entireWindowShot);

    // ------ scall down NSImage

    NSBitmapImageRep *imageRep = [[NSBitmapImageRep alloc] initWithCGImage:screenShot];
    NSData *tiffData = imageRep.TIFFRepresentation;
    NSImage *image = [[NSImage alloc] initWithData:tiffData];
    
    NSSize newSize = NSMakeSize(cg.size.width * this->captureImageScale,
                                cg.size.height * this->captureImageScale);
    NSImage *smallImage = [[NSImage alloc] initWithSize: newSize];

    // Report an error if the source isn't a valid image
    if (![image isValid]){
        std::cout << "Invalid Image" << std::endl;
        return false;
    } else {
        [smallImage lockFocus];
        [image setSize: newSize];
        [[NSGraphicsContext currentContext] setImageInterpolation:NSImageInterpolationHigh];
        [image drawAtPoint:NSZeroPoint fromRect:CGRectMake(0, 0, newSize.width, newSize.height) operation:NSCompositeCopy fraction:1.0];
        [smallImage unlockFocus];
    }
    CFRelease(imageRep);
    CFRelease(image);
    CFRelease(screenShot);

    
    if(!exportNSImageToPNGFile(smallImage, index))
    {
        std::cout << "Error : exportNSImageToPNGFile failed\n";
    }
    
    CFRelease(smallImage);
    
    
    // inform the completion
    if(this->exportScreenshotCompleted != nullptr)
        this->exportScreenshotCompleted();
    
    return true;
}


bool IRHeavyWeightComponent::exportNSImageToPNGFile(void* nsImage, int id)
{

    NSImage* smallImage = static_cast<NSImage*>(nsImage);

    NSData *smallData = smallImage.TIFFRepresentation;
    NSBitmapImageRep *smallRep = [NSBitmapImageRep imageRepWithData:smallData];
    CGImageRef smallScreenShot = smallRep.CGImage;

    // --------- export screenshot as a png file

    NSString *indexS = [NSString stringWithFormat:@"_%d",id];
    NSString* path1 = @"/Users/keitaro/Desktop/workspace";
    NSString* path2 = @".png";

    NSString* path = [NSString stringWithFormat:@"%@%@%@", path1,
    indexS, path2];

    std::cout << path << " creating...\n";

    CFURLRef url = (__bridge CFURLRef)[NSURL fileURLWithPath:path];
    CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);

    if (!destination) {
      std::cout << "failed to create destination\n";
      return false;
    }

    CGImageDestinationAddImage(destination, smallScreenShot, nil);

    if (!CGImageDestinationFinalize(destination)) {
      std::cout << "failed to write image\n";
      CFRelease(destination);
      return false;
    }
    
    CFRelease(destination);
    return true;
}

void IRHeavyWeightComponent::converNSImageToJUCEImage(void* nsImage, Image* juceImage)
{
    
}


void IRHeavyWeightComponent::exportOpenGLPixels(int index, juce::Component* target)
{
    
    auto owner = target;
    //std::cout << "exportScreenshot : " << this->component << std::endl;
    auto* peer = owner->getPeer();
    if(peer == nullptr)
    {
        return;
    }
    
    auto b = owner->getBounds();
    // get current display and check the scale in case using Retina Display which has double resolution.
    auto display = Desktop::getInstance().getDisplays().findDisplayForRect(this->component->getScreenBounds());
    auto scale = display.scale;

    //std::cout << "screenshot scale = " << scale << std::endl;
    int leftMargin = 55;
    int topMargin = 70;
    int x = leftMargin * scale;
    int y = topMargin * scale;
    int w = (b.getWidth()) * scale;
    int h = (b.getHeight()) * scale;

    //CGRect cg = CGRectMake(x, y, w, h);
    
    
    // Make the BYTE array, factor of 3 because it's RBG.
    unsigned char* pixels = new unsigned char[3 * w * h];

    /*
    
    this->ctx.extensions.glReadPixels(GLint(x), GLint(y),
                                      GLsizei(w), GLsizei(h),
                                      GL_RGB, GL_UNSIGNED_BYTE,
                                      pixels);
     */
    
    //USE_FUNCTION (glReadPixels,     void, (GLint p1, GLint p2, GLsizei p3, GLsizei p4, GLenum p5, GLenum p6, void * p7), (p1, p2, p3, p4, p5, p6, p7))


    /*
    // Convert to FreeImage format & save to file
    FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
    FreeImage_Save(FIF_BMP, image, "C:/test.bmp", 0);
*/
    // Free resources
    //FreeImage_Unload(image);
    delete [] pixels;
}
