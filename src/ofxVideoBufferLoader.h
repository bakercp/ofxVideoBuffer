/*==============================================================================
 
 Copyright (c) 2010, 2011, 2012 Christopher Baker <http://christopherbaker.net>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 ==============================================================================*/

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




