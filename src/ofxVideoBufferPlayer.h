/* christopher baker */
/* http://christopherbaker.net */

#pragma once

#include "ofMain.h"
//#include "ofxVideoBufferTypes.h"
#include "ofxVideoBuffer.h"
#include "ofxVideoPlayer.h"

// TODO:
// video loop event
// - move video backed buffer to ofxvideobuffer.
// - the ofxVideoBufferPlayer should only be a video buffer player.

enum ofxVideoBufferPlayerSourceType {
    OFX_VIDEO_PLAYER_SRC_TYPE_NONE        = 0,
    OFX_VIDEO_PLAYER_SRC_TYPE_IMAGE       = 1,
    OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOPLAYER = 2,
    OFX_VIDEO_PLAYER_SRC_TYPE_VIDEOBUFFER = 3,
};

class ofxVideoBufferPlayer : public ofBaseVideoDraws {
public:
    ofxVideoBufferPlayer(); 
    virtual ~ofxVideoBufferPlayer();

    //////////////////////////////////////////////////
    // Frame Access //////////////////////////////////
    ofxVideoFrame& getCurrentFrame(); // retrieve a reference to the current frame

    //////////////////////////////////////////////////
    // ofBaseVideo ///////////////////////////////////
    bool isFrameNew();
	void close();

    //////////////////////////////////////////////////
    // ofBaseHasPixels ///////////////////////////////
    unsigned char * 	getPixels();
    ofPixelsRef			getPixelsRef();

    //////////////////////////////////////////////////
    // ofBaseUpdate //////////////////////////////////
	void update();  // like idle(), claculates the current frame position
                    // for the player based on real-time values

    //////////////////////////////////////////////////
    // ofBaseHasTexture //////////////////////////////
    ofTexture & getTextureReference();
	void setUseTexture(bool bUseTex);

    //////////////////////////////////////////////////
    // ofBaseDraws ///////////////////////////////////
    void  draw(float x,float y);
	void  draw(float x,float y,float w, float h);
	float getHeight();
	float getWidth();
	void  setAnchorPercent(float xPct, float yPct);
    void  setAnchorPoint(float x, float y);
	void  resetAnchor();
    
    //////////////////////////////////////////////////
    // Source ////////////////////////////////////////
    void loadImage(string filename);
    void loadMovie(string filename);
    void loadVideoBuffer(ofxVideoBuffer* buffer); // set the internal buffer to the pointer

    ofxVideoFrame&  getImagePlayer();
    ofxVideoPlayer& getVideoPlayer();   // get the internal video player
    ofxVideoBuffer* getVideoBuffer();   // get the internal buffer pointer
    
    bool    isImagePlayer();
    bool    isVideoPlayer();
    bool    isBufferPlayer();
    
    bool    isLoaded();           // checks for a NULL buffer
    bool    isEmpty();            // is the internal buffer's count == 0

    int     getCount();
    int     getSize();        
    bool    isFull();         
    float   getPctFull();

    //////////////////////////////////////////////////
    // Looping ///////////////////////////////////////
    void setLoopPoints(int start, int end);
    void setLoopPointStart(int frame);
    void setLoopPointEnd(int frame);

    void setLoopPointsNorm(float startn, float endn);
    void setLoopPointStartNorm(float framen);
    void setLoopPointEndNorm(float framen);
    
	int getLoopPointStart();
    int getLoopPointEnd();

    float getLoopPointStartNorm();
    float getLoopPointEndNorm();

    void clearLoopPoints();
    
    void setLoopType(ofLoopType _loopType);
	ofLoopType getLoopType();
    
    //////////////////////////////////////////////////
    // Positioning ///////////////////////////////////
    void  setFrame(int frame);
    void  setFrameNorm(float framen);
    int   getFrame();
    float getFrameNorm();
    
    //////////////////////////////////////////////////
    // Playback //////////////////////////////////////

    void  start();
    void  stop();
    bool  isPlaying();
    
    void  setPaused(bool bIsPaused);
    bool  isPaused();
    
    void  setSpeed(float speed);
    float getSpeed();
    
    float getFrameRate(); 
    float getFrameDuration();

    //////////////////////////////////////////////////
    // Utility ///////////////////////////////////////
    
    string toString();
    
    
protected:

    ofxVideoFrame  emptyFrame;      // the empty frame
    
    ofxVideoFrame   image;
    ofxVideoBuffer* buffer;         // the pointer to the current video buffer 
    ofxVideoPlayer  player;         // the pointer to the current video player 
                                    // if the player is playing from disk, it must
                                    // own its own ofVideoPlayer.  ofVideoPlayer does
                                    // not take kindly to multi-tap frames.

    ofxVideoBufferPlayerSourceType   sourceType; // the type of the source
    
    // playback
    bool  bIsPlaying;               // is stopped?
    bool  bIsPaused;                // is paused?
    float speed;                    // the speed scaler
    float playDirection;            // forward or backward (for palindrome)
    
    // frame indicies, times, etc
    bool  bIsFrameNew;
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
	
    int bufferMod(int frame);
    
    int     frameNormToFrame(float n);
    float   frameToFrameNorm(int   frame);
    
};