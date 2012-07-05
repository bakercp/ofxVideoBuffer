//
//  ofxVideoPlayer.h
//  ofxVideoBufferExample
//
//  Created by Christopher Baker on 7/5/12.
//  Copyright (c) 2012 School of the Art Institute of Chicago. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxVideoBufferTypes.h"

class ofxVideoPlayer : public ofVideoPlayer {
public:
    ofxVideoPlayer() : ofVideoPlayer() {}
    virtual ~ofxVideoPlayer() {}
    
    ofxVideoFrame& getFrame() {
        if(isFrameNew()) {
            // cache a new frame
            frame = ofxVideoFrame(new ofImage());
            frame->setFromPixels(getPixelsRef());
        } else {
            return frame;
        }
    }
    
private:
    
    ofxVideoFrame frame;
    
};
