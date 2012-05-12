#pragma once

#include "ofMain.h"
#include "ofxVideoBufferTypes.h"

enum ofxVideoBufferLoaderState {
    OFX_VID_BUFFER_IDLE,
    OFX_VID_BUFFER_LOADING,
    OFX_VID_BUFFER_COMPLETE
};

class ofxVideoBufferLoader : protected ofThread {
	
public:
	
    ofxVideoBufferLoader();
    virtual ~ofxVideoBufferLoader();    

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
	ofVideoPlayer player;
    
    // thread functions
    void threadedFunction();
    
    ofxVideoBufferLoaderState state;
};




