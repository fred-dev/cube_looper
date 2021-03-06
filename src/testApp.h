/*
 *  ofxAlphaMaskShader.h
 *
 * Created by James George, http://www.jamesgeorge.org
 * in collaboration with FlightPhase http://www.flightphase.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 * ofxAlphaMaskShader is not really an addon, but an example
 * of how to use a shader to have one image become the alpha
 * channel of another.
 */

#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxSimpleGuiToo.h"
#include "ofxGamepadHandler.h"
#include "ofxSimpleGuiToo.h"

class testApp : public ofBaseApp{

  public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    void startRecording(int sliceNumber);
    void stopRecording(int sliceNumber);
    
    void axisChanged(ofxGamepadAxisEvent &e);
    void buttonPressed(ofxGamepadButtonEvent &e);
    void buttonReleased(ofxGamepadButtonEvent &e);
    ofxGamepad gbuttons;
	float camDistance;
    ofEasyCam cam; // add mouse controls for camera movement
    
    ofVec3f orientation;
    int sliceCount;
    ofVideoGrabber 			vidGrabber;
    ofPtr<ofQTKitGrabber>	vidRecorder;
    
    ofQTKitPlayer   recordedVideoPlayback[8];
    int loopStamp[8];
    bool readyToPlay[8];
    bool masterLoopRecorded;
    float masterDuration;
    int loopCount[8];
    bool hasFirstLoop[8];
    bool isReRecording[8];
    
    
    void videoSaved(ofVideoSavedEventArgs& e);
	ofFile file;
    vector<string> videoDevices;
    vector<string> audioDevices;
    vector<string> videoCodecs;
    vector<string> audioCodecs;
    
    bool bLaunchInQuicktime;
};

#endif
