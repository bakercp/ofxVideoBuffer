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
#include "ofxVideoBufferData.h"
#include "ofxVideoBufferLoader.h"

// buffer types
enum ofxVideoBufferType {
    OFX_VIDEO_BUFFER_FIXED,
    OFX_VIDEO_BUFFER_CIRCULAR,
    OFX_VIDEO_BUFFER_PASSTHROUGH,
};

class ofxVideoBuffer {
public:
    // a buffer always has at least one frame
    ofxVideoBuffer();
    ofxVideoBuffer(int _bufferSize); 
    ofxVideoBuffer(int _bufferSize, ofxVideoBufferType _type);
    virtual ~ofxVideoBuffer();    

    void  update(ofEventArgs & args);  // notify in update so the notification is thread safe
    
    void  loadImage(string _filename);
    void  loadMovie(string _filename);  // this will load a video into the buffer in another thread
	void  loadMovie(string _filename, int _startFrame, int _endFrame); // "
    bool  isLoading();             // checks to see if the video is loading
    
    //bool  bufferFrame(const ofPixels& pixels);  // frames are added to the buffer here
    bool bufferFrame(const ofxVideoFrame& frame);
    
    ofxVideoFrame operator [](int i) const;  // frames are accessed via the [] operator
    ofxVideoFrame at(int i) const;           // 
    
    void  clear();  // resets the count.  does not clear or resize the underlying buffer
    bool  isEmpty() const; // is the count == 0?
    
    // counts and sizes
    int   getCount() const; // the number of valid frames i.e. (head + 1)

    bool  setSize(int maxFrames); // set maximum buffer size (will not clear)
    int   getSize() const;        // get maximum buffer size
    
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
    
    
    string toString() {
        stringstream ss;
        ss << "ofxVideoBuffer:" << endl;
        if(isFixedBuffer()) {
            ss << "\tTYPE=FIXED" << endl;
        } else if(isPassthroughBuffer()) {
            ss << "\tTYPE=PASSTHROUGH" << endl;
        } else if(isCircularBuffer()) {
            ss << "\tTYPE=CIRCULAR" << endl;
        }
        ss << "\tFrameRate="<< getFrameRate() << endl;
        ss << "\t%Full="<< getPercentFull() << endl;
        ss << "\tSize="<< getSize() << endl;
        ss << "\tCount="<< getCount() << endl;
        ss << "\tFrameRate="<< getFrameRate() << endl;
        ss << "\tIsLoading="<< isLoading() << endl;
        ss << "\tIsReadOnly="<< isReadOnly() << endl;
        
        return ss.str();
        
        
    }

protected:
    bool                 readOnly; // just a loose flag for preventing writes
    
    int                  count;  // the count is the number of "valid" frames in the buffer
    ofxVideoBufferType   mode;   // the buffer mode (circular, etc)
    ofxVideoBufferData   buffer; // frames are stored in this buffer
    
    
    ofxVideoBufferLoader loader; // a loader for loading whole video clips into buffer
                                 // in another thread.  the update function in 
                                 // this class checks on that loader and uploads the 
                                 // pixels to textures from the main thread.
    
    bool    bUseTexture;
    
    float frameRate;
};




