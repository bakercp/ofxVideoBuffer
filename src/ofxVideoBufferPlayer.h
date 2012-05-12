/* christopher baker */
/* http://christopherbaker.net */

#pragma once

#include "ofMain.h"
#include "ofxVideoBufferTypes.h"
#include "ofxVideoBuffer.h"

// TODO:
// video loop event

class ofxVideoBufferPlayer {

public:
    
    ofxVideoBufferPlayer(); 
    virtual ~ofxVideoBufferPlayer();

    void loadBuffer(ofxVideoBuffer* buffer); // set the internal buffer to the pointer
    void unloadBuffer();                     // set the internal buffer to NULL
    
    ofxVideoBuffer* getVideoBuffer();        // get the internal buffer pointer
    bool isBufferLoaded();                   // checks for a NULL buffer
    bool isBufferEmpty();                    // is the internal buffer's count == 0
    
	void update();                           // like idle(), claculates the current frame position
                                             // for the player based on real-time values

    ofxVideoFrame& getFrame();               // gets the current frame reference

    void draw(int x, int y);                 // draw the current frame
    
    // frame based loop control
	void setLoopPoints(int start, int end);
    bool setLoopPointsStart(int frame);
    bool setLoopPointsEnd(int frame);

	int getLoopPointsStart();
    int getLoopPointsEnd();

    // frame based positioning
    void setPosition(int frame);
    
    // getting position
    int   getPosition();

    // clear loops!
    void clearLoopPoints();

    void       setLoopType(ofLoopType _loopType);
	ofLoopType getLoopType();
    
    void  setSpeed(float speed);
    float getSpeed();
    
    void  setFPS(float fps);
    float getFPS();
    
    float getFrameDuration();
    
    string toString();
    
private:
    
    ofxVideoBuffer* buffer;     // the pointer to the current video buffer 

    // playback
    float speed;                // the speed scaler
    float fps;                  // the fps assumed for cacluations, works w/ fps
    float playDirection;        // forward or backward (for palindrome)
    
    // frame indicies, times, etc
    int   lastFrame;                // the last frame that was pushed
    int   currentFrame;             // the current frame
    float lastFramePushTime;        // the time when the last frame was pushed, (ms)
    float lastFrameFraction;        // the fraction of the frame that will be sent
                                    // with the next batch (i.e. if if our pushed
                                    // frame calculation ended up in the middle 
                                    // of a frame.)
    // looping        
    ofLoopType loopType;            // the player's loop type
    
	int loopPointsStart;            // the loop start point
	int loopPointsEnd;              // the loop stop  point
	
};

