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

#include <set>
#include "ofMain.h"
#include "ofxVideoFrame.h"
#include "ofxVideoBufferData.h"
#include "ofxVideoBufferLoader.h"

// buffer types
enum ofxVideoBufferType {
    OFX_VIDEO_BUFFER_FIXED,
    OFX_VIDEO_BUFFER_CIRCULAR,
    OFX_VIDEO_BUFFER_PASSTHROUGH,
};

class ofxVideoBuffer;

class ofxVideoBufferListener {
public:
    virtual void bufferSizeChanged(ofxVideoBuffer* buffer) = 0;
    virtual void bufferCleared(ofxVideoBuffer* buffer) = 0;
    virtual void bufferLoadComplete(ofxVideoBuffer* buffer) = 0;
};

class ofxVideoBuffer {
public:
    // a buffer always has at least one frame
    ofxVideoBuffer();
    ofxVideoBuffer(int _bufferSize); 
    ofxVideoBuffer(int _bufferSize, ofxVideoBufferType _type);
    virtual ~ofxVideoBuffer();    

    void  update(ofEventArgs & args);  // notify in update so the notification is thread safe
    void  update();
    
    void  loadImageAsync(const string& _filename);
    void  loadMovieAsync(const string& _filename);  // this will load a video into the buffer in another thread
	void  loadMovieAsync(const string& _filename, int _startFrame, int _endFrame); // "

    bool  isLoaded() const;
    bool  isLoading() const;             // checks to see if the video is loading
    
    float getPercentLoaded() const;
    
    ofxVideoBufferLoaderState getState() const;

    ofxVideoBufferLoader& getLoaderRef();

    //bool  bufferFrame(const ofPixels& pixels);  // frames are added to the buffer here
    bool bufferFrame(const ofxSharedVideoFrame& frame);
    
    ofxSharedVideoFrame operator [](int i) const;  // frames are accessed via the [] operator
    ofxSharedVideoFrame at(int i) const;           // 
    
    void  clear();  // resets the count.  does not clear or resize the underlying buffer
    bool  isEmpty() const; // is the count == 0?
    
    // counts and sizes
    size_t getCount() const; // the number of valid frames i.e. (head + 1) - count

    bool   setSize(int maxFrames); // set maximum buffer size (will not clear)
    size_t getSize() const;        // get maximum buffer size (the size() of the underlying ofxVideoBufferData)
    
    bool  isFull() const;         // do all frames hold valid data?
    float getPercentFull() const; // how full is the buffer
    
    // buffer modes
    void setBufferType(ofxVideoBufferType type);
    ofxVideoBufferType getBufferType() const;

    bool isFixedBuffer() const;        // the normal buffer mode (i.e. fixed size)
    bool isPassthroughBuffer() const;   // a passthrough buffer
    bool isCircularBuffer() const;      // a buffer of fixed size, FIFO style
    
    // TODO: buffer wide texture use?  texture only as needed?
    // void setUseTexture(bool usesTexture);
    // bool bUsesTextures; // the buffer uses textures

    void  setUseTexture(bool bUseTex);

    float getFrameRate();
    void  setFrameRate(float frameRate);
    
    bool  isReadOnly();
    bool  isWritable();
    void  setReadOnly(bool readOnly);
    
    string toString();

    // listener interactions
    
    void reportSizeChanged();
    void reportCleared();
    void reportLoadComplete();
    
    bool hasListener(ofxVideoBufferListener* listener);
    bool addListener(ofxVideoBufferListener* listener);
    bool removeListener(ofxVideoBufferListener* listener);
    
protected:
    
    set< ofxVideoBufferListener* >::iterator listenersIter;
    set< ofxVideoBufferListener* > listeners;
    
    
    bool                 readOnly; // just a loose flag for preventing writes
    
    size_t               count;  // the count is the number of "valid" frames in the buffer
    ofxVideoBufferType   mode;   // the buffer mode (circular, etc)
    ofxVideoBufferData   buffer; // frames are stored in this buffer
    
    
    ofxVideoBufferLoader loader; // a loader for loading whole video clips into buffer
                                 // in another thread.  the update function in 
                                 // this class checks on that loader and uploads the 
                                 // pixels to textures from the main thread.
    
    bool bUseTexture;
    
    float frameRate;
};

typedef ofPtr < ofxVideoBuffer > ofxSharedVideoBuffer;