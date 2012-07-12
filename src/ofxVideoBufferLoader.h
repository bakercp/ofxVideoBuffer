#pragma once

#include "ofMain.h"
#include "ofxVideoFrame.h"
#include "ofxVideoPlayer.h"
#include "ofxVideoBufferData.h"

enum ofxVideoBufferLoaderState {
    OFX_VID_BUFFER_IDLE     = 0,
    OFX_VID_BUFFER_LOADING  = 1,
    OFX_VID_BUFFER_COMPLETE = 2
};

enum ofxVideoBufferLoadType {
    OFX_VID_BUFFER_LOAD_VIDEO = 0,
    OFX_VID_BUFFER_LOAD_IMAGE = 1,
//    OFX_VID_BUFFER_LOAD_IMAGE_SEQUENCE = 2,
};

class ofxVideoBufferLoader : protected ofThread {
	
public:
	
    ofxVideoBufferLoader();
    virtual ~ofxVideoBufferLoader();    

    void loadImage(ofxVideoBufferData* _buffer, string _filename);
	void loadMovie(ofxVideoBufferData* _buffer, string _filename);
	void loadMovie(ofxVideoBufferData* _buffer, string _filename, int _startFrame, int _endFrame);

	void cancelLoad();

    void reset();
    
    float getPercentLoaded();

    bool isIdle();
    bool isLoading();
    bool isComplete();
    
    ofxVideoBufferLoaderState getState();
    
    int getStartFrame()   const;
    int getEndFrame()     const;
    int getCurrentFrame() const;
    
protected:
        
    int currentFrame;
    int startFrame;
    int endFrame;
    
	ofxVideoBufferData* buffer;
	ofxVideoPlayer player;
    ofxVideoFrame image;
    
    // thread functions
    void threadedFunction();
    
    ofxVideoBufferLoaderState state;
    ofxVideoBufferLoadType    loadType;
};




