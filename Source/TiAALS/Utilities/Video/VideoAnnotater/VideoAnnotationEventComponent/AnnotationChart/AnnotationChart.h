//
//  AnnotationChart.h
//  IRiMaS
//
//  Created by Keitaro on 02/07/2020.
//

#ifndef AnnotationChart_h
#define AnnotationChart_h


class AnnotationChart : public Component,
public IRStrComponent
{
public:
    
    struct annotationData
    {
        float beginTime = 0;
        float endTime = 0;
        float duration = 0;
        
        annotationData(float beginTime, float endTime)
        {
            this->beginTime = beginTime;
            this->endTime = endTime;
            this->duration = this->endTime - this->beginTime;
        }
        void setBeginTime(float val)
        {
            this->beginTime = val;
            this->duration = this->endTime - this->beginTime;
        }
        
        void setEndTime(float val)
        {
            this->endTime = val;
            this->duration = this->endTime - this->beginTime;
        }
    };

    struct annotationGraphStr
    {
        int size = 0;
        int maxCount = 0;
        std::vector<int> count;
        
        void clear()
        {
            count.clear();
            size = 0;
        }
        
        void allocateWith(int n, int val)
        {
            clear();
            size = n;
            maxCount = 0;
            std::vector<int> v(n, val);
            count = v;
        }
        
        bool isEmpty() const
        {
            if(this->maxCount == 0) return true;
            else return false;
        }
    };

    AnnotationChart(IRStr* str) : IRStrComponent(str)
    {
        
    }
    
    ~AnnotationChart()
    {
        this->graph.clear();
    }
    // ==============================

    void resized() override
    {
        reCalc();
    }
    
    void paint(Graphics& g) override
    {
        
        g.fillAll(Colours::transparentBlack);
        
        g.setColour(getStr()->SYSTEMCOLOUR.contents);

        // check if the graph contents is NOT empty
        if(!this->graph.isEmpty())
        {
            int max = this->graph.maxCount;
            for(int i = 0; i < this->graph.size; i ++)
            {
                int norm = floor(((float)this->graph.count[i] / (float)max) * getHeight());

                g.drawLine(i, getHeight(), i, getHeight() - norm, 1.0);
            }
        }
    }
    
    // ==============================
    void init()
    {
        this->graph.clear();
        this->annotations.clear();
    }
    
    // ==============================
    
    void addEvent(float beginTime, float endTime)
    {
        this->annotations.push_back(annotationData(beginTime, endTime));
    }
    
    void reCalc()
    {
        this->graph.clear();
        this->graph.allocateWith(getWidth(), 0);
        
        float w = (float) getWidth();
        //float h = (float) getHeight();
        float exposedVideoLength = this->videoEndTimeCode - this->videoBeginTimeCode;
        // calc seconds per pixel
        //float SecPerPix = this->videoLength / w;
        float SecPerPix = exposedVideoLength / w;
        for(auto a : this->annotations)
        {
            
            // fix the begin time following to the user defined videoBeginTimeCode
            float begin = a.beginTime - this->videoBeginTimeCode;
                    
            if(begin < 0) begin = 0;
            
            int beginIndex = floor((begin) / SecPerPix);
                
            // fix the end time following to the user defined videoEndTimeCode
            float end = a.endTime - this->videoBeginTimeCode;
            
            if(end > this->videoEndTimeCode) end = this->videoEndTimeCode;
            if(end < 0) end = begin;
                
            int duration = floor((end - begin) / SecPerPix);
            
                //std::cout << "videoBeginTimeCode = " << this->videoBeginTimeCode << ", endTimeCode = " << this->videoEndTimeCode << " : duration = " << exposedVideoLength << std::endl;
                //std::cout << "reCalc : begin = " << beginIndex << " : end = " << end << " : duration = " << duration << " : exposedVideoLength = " << exposedVideoLength << " : width = " << w << std::endl;
            
            int endIndex = beginIndex+duration;
            if(endIndex > w){
                std::cout << "endIndex is larger than width, " << endIndex << " : width " << w << std::endl;
                endIndex = w;
            }
                
                for(int i=beginIndex;i<endIndex;i++)
                {
                    this->graph.count[i] ++;
                    
                    if(this->graph.maxCount < this->graph.count[i])
                    {
                        this->graph.maxCount = this->graph.count[i];
                    }
                }
        }
        
    }
    
    // ==============================
    
    void setVideoLength(float length)
    {
        this->videoLength = length;
        this->videoEndTimeCode = length;
    }

    void setVideoBeginEndTiemCode(float begin, float end)
    {
        this->videoBeginTimeCode = begin;
        this->videoEndTimeCode = end;
    }
    // ==============================

private:
    
    std::vector<annotationData> annotations;
    float videoLength = 0.0;
    
    // user defined begin and end time code
    float videoBeginTimeCode = 0.0;
    float videoEndTimeCode = 0.0;
    
    annotationGraphStr graph;
};


#endif /* AnnotationChart_h */
