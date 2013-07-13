
#include "testApp.h"
#include "ofxGamepadHandler.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofxGamepadHandler::get()->enableHotplug();
	
	//CHECK IF THERE EVEN IS A GAMEPAD CONNECTED
	if(ofxGamepadHandler::get()->getNumPads()>0){
		ofxGamepad* pad = ofxGamepadHandler::get()->getGamepad(0);
		ofAddListener(pad->onAxisChanged, this, &testApp::axisChanged);
		ofAddListener(pad->onButtonPressed, this, &testApp::buttonPressed);
		ofAddListener(pad->onButtonReleased, this, &testApp::buttonReleased);
	}
	ofEnableAlphaBlending();
    ofSetVerticalSync(true);
	for (int i=0; i<8; i++) {
        readyToPlay[i]=true;
        hasFirstLoop[i]=false;
        }
    masterLoopRecorded=false;
    vidRecorder = ofPtr<ofQTKitGrabber>( new ofQTKitGrabber() );
    vidGrabber.setGrabber(vidRecorder);
    videoDevices = vidRecorder->listVideoDevices();
    audioDevices = vidRecorder->listAudioDevices();
    //vidRecorder->setUseAudio(false);
	ofAddListener(vidRecorder->videoSavedEvent, this, &testApp::videoSaved);

    vidGrabber.listDevices();
    vidGrabber.setDeviceID(2);
    vidRecorder->setAudioDeviceID(6);
    vidGrabber.initGrabber(1920,1080);
    vidRecorder->initGrabberWithoutPreview();
    
    videoCodecs = vidRecorder->listVideoCodecs();
    for(size_t i = 0; i < videoCodecs.size(); i++){
        ofLogVerbose("Available Video Codecs") << videoCodecs[i];
    }
    
    audioCodecs = vidRecorder->listAudioCodecs();
    for(size_t i = 0; i < audioCodecs.size(); i++){
        ofLogVerbose("Available Video Codecs") << videoCodecs[i];
    }
    
    //vidRecorder->setVideoCodec("QTCompressionOptionsHD1080SizeH264Video");
    //vidRecorder->setVideoCodec("QTCompressionOptionsLosslessAnimationVideo");
    vidRecorder->setVideoCodec("QTCompressionOptionsLosslessAppleIntermediateVideo");
    //vidRecorder->setVideoCodec("QTCompressionOptionsJPEGVideo");
    vidRecorder->setAudioCodec("QTCompressionOptionsHighQualityAACAudio");
    vidRecorder->initRecording();
    sliceCount=0;
    
    gui.addSlider("Easy Cam Distance", camDistance, 0, 5000);
    gui.addSlider("OrienationX", orientation.x, 0, 360);
    gui.addSlider("OrienationY", orientation.y, 0, 360);
    gui.addSlider("OrienationZ", orientation.z, 0, 360);
    
    gui.loadFromXML();
    gui.show();
    //cam.setDistance(0);

	
}   

//--------------------------------------------------------------
void testApp::update(){
    //vidGrabber.update();
    for (int i =0; i<8; i++) {
        if(recordedVideoPlayback[i].isLoaded()){
            recordedVideoPlayback[i].update();
            if (recordedVideoPlayback[i].getIsMovieDone()==true) {
                 readyToPlay[i]=true;
            }
            if (loopStamp[i]==recordedVideoPlayback[0].getCurrentFrame()&&readyToPlay[i]==true) {
                recordedVideoPlayback[i].play();
                readyToPlay[i]=false;
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    glEnable(GL_DEPTH_TEST);
    
    cam.setScale(1,-1,1);
	ofBackground(0, 0, 0);
    for (int i=0; i<4; i++) {
        if(recordedVideoPlayback[i].isLoaded()&&isReRecording[i]==false){
            cam.setDistance(camDistance);
            cam.begin();
            if (cam.getMouseInputEnabled()==false) {
                cam.setOrientation(orientation);
            }
            ofPushMatrix();
            ofTranslate(-recordedVideoPlayback[0].getWidth()/2, -recordedVideoPlayback[0].getHeight()/2,-recordedVideoPlayback[0].getWidth()/2);
            ofSetHexColor(0xFFFFFF);
            if (i==0) {
                ofPushMatrix();
                recordedVideoPlayback[i].draw(0,0);
                ofPopMatrix();
            }
            
            if (i==1) {
                ofPushMatrix();
                ofRotateY(270);
                recordedVideoPlayback[i].draw(0,0);
                ofPopMatrix();
            }
            
            if (i==2) {
                ofPushMatrix();
                ofTranslate(0,0,recordedVideoPlayback[i].getWidth());
                recordedVideoPlayback[i].draw(0,0);
                ofPopMatrix();
            }
            
            if (i==3) {
                ofPushMatrix();
                ofTranslate(recordedVideoPlayback[i].getWidth(),0,0);
                ofRotateY(270);
                recordedVideoPlayback[i].draw(0,0);
                ofPopMatrix();
            }
            
            ofPopMatrix();
            cam.end();
        }
    }

    string text=ofToString(ofGetFrameRate());
    ofDrawBitmapString(text, 10, 10);
	//then draw a quad for the top layer using our composite shader to set the alpha
	ofPushStyle();
    ofSetColor(255);
    ofDrawBitmapString("' ' space bar to toggle recording", 1600, 980);
    ofDrawBitmapString("'v' switches video device", 1600, 1000);
    ofDrawBitmapString("'a' swiches audio device", 1600, 1020);
    
    //draw video device selection
    ofDrawBitmapString("VIDEO DEVICE", 20, 540);
    for(int i = 0; i < videoDevices.size(); i++){
        if(i == vidRecorder->getVideoDeviceID()){
			ofSetColor(255, 100, 100);
        }
        else{
            ofSetColor(255);
        }
        ofDrawBitmapString(videoDevices[i], 20, 560+i*20);
    }
    
    //draw audio device;
    int startY = 580+20*videoDevices.size();
    ofDrawBitmapString("AUDIO DEVICE", 20, startY);
    startY += 20;
    for(int i = 0; i < audioDevices.size(); i++){
        if(i == vidRecorder->getAudioDeviceID()){
			ofSetColor(255, 100, 100);
        }
        else{
            ofSetColor(255);
        }
        ofDrawBitmapString(audioDevices[i], 20, startY+i*20);
    }
    
//    ofDrawBitmapString("VIDEO CODECS", 1700, 540);
//    for(int i = 0; i < videoCodecs.size(); i++){
//        ofSetColor(255);
//        ofDrawBitmapString(videoCodecs[i], 1700, 560+i*20);
//    }
//    
//    //draw audio device;
//    int startY2 = 700+20*videoCodecs.size();
//    ofDrawBitmapString("AUDIO CODEC", 1700, startY);
//    startY2 += 20;
//    for(int i = 0; i < audioCodecs.size(); i++){
//        ofSetColor(255);
//        ofDrawBitmapString(audioCodecs[i], 1700, startY+i*20);
//    }

    ofPopStyle();
    gui.draw();
    //vidGrabber.draw(0, 0, 1920/8, 1080/8);
}
void testApp::startRecording(int sliceNumber){
    if(recordedVideoPlayback[sliceNumber].isLoaded()){
        recordedVideoPlayback[sliceNumber].stop();
        //recordedVideoPlayback[sliceNumber].close();
        file.removeFile("Slice" + ofToString(sliceNumber+1)+ ".mov");
    }
    if (hasFirstLoop[sliceNumber]==true) {
        isReRecording[sliceNumber]=true;
    }
    if (hasFirstLoop[sliceNumber]==false) {
        hasFirstLoop[sliceNumber]=true;
    }
    
    loopStamp[sliceNumber]=recordedVideoPlayback[0].getCurrentFrame();
    vidRecorder->startRecording("Slice" + ofToString(sliceNumber+1)+ ".mov");
}

void testApp::stopRecording(int sliceNumber){
    sliceCount=sliceNumber;
    if(vidRecorder->isRecording()){
        vidRecorder->stopRecording();
        isReRecording[sliceNumber]=false;
    }
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if (key=='1') {
        if(masterLoopRecorded==true){
            for (int i=0; i<8; i++) {
                recordedVideoPlayback[i].stop();
                recordedVideoPlayback[i].close();
                file.removeFile("Slice" + ofToString(i+1)+ ".mov");
                hasFirstLoop[i]=false;
            }
            
            file.removeFile("Slice1.mov");
        }
        
        vidRecorder->startRecording("Slice1.mov");
    }
    if (key=='2') {
        startRecording(1);
    }
    if (key=='3') {
        startRecording(2);
    }
    if (key=='4') {
        startRecording(3);
    }
    if (key=='5') {
        startRecording(4);
    }
    if (key=='6') {
        startRecording(5);
    }
    if (key=='7') {
        startRecording(6);
    }
    if (key=='8') {
        startRecording(7);
    }
	if(key == ' '){
        if (sliceCount<7) {
            if(recordedVideoPlayback[sliceCount+1].isLoaded()){
                recordedVideoPlayback[sliceCount+1].close();
            }
        }
        if (sliceCount==7) {
            if(recordedVideoPlayback[0].isLoaded()){
                recordedVideoPlayback[0].close();
            }
        }
        
        vidRecorder->startRecording("Slice" + ofToString(sliceCount+1)+".mov");
    }
    if (key=='g') {
        gui.toggleDraw();
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
    if (key=='1') {
        masterLoopRecorded=true;
        stopRecording(0);
    }
    if (key=='2') {
        stopRecording(1);
    }
    if (key=='3') {
        stopRecording(2);
    }
    if (key=='4') {
        stopRecording(3);
    }
    if (key=='5') {
        stopRecording(4);
    }
    if (key=='6') {
        stopRecording(5);
    }
    if (key=='7') {
        stopRecording(6);
    }
    if (key=='8') {
        stopRecording(7);
    }
    
    
	if(key == 'v'){
		vidRecorder->setVideoDeviceID( (vidRecorder->getVideoDeviceID()+1) % videoDevices.size() );
    }
	if(key == 'a'){
        vidRecorder->setAudioDeviceID( (vidRecorder->getAudioDeviceID()+1) % audioDevices.size() );
    }
    if(key == ' '){
        if(vidRecorder->isRecording()){
            vidRecorder->stopRecording();
            sliceCount++;
            if (sliceCount>7) {
                sliceCount=0;
            }
        }
    }
}
void testApp::axisChanged(ofxGamepadAxisEvent& e)
{
	cout << "AXIS " << ofToString(e.axis) << " VALUE " << ofToString(e.value) << endl;
}
void testApp::buttonPressed(ofxGamepadButtonEvent& e)
{
    
    if (e.button==0) {
        stopRecording(0);
    }
    if (e.button==1) {
        stopRecording(1);
    }
    if (e.button==2) {
        stopRecording(2);
    }
    if (e.button==3) {
        stopRecording(3);
    }
    if (e.button==4) {
        stopRecording(4);
    }
    if (e.button==5) {
        stopRecording(5);
    }
    if (e.button==6) {
        stopRecording(6);
    }
    if (e.button==7) {
        stopRecording(7);
    }
	cout << "BUTTON " << ofToString(e.button) << " RELEASED" << endl;
}
void testApp::buttonReleased(ofxGamepadButtonEvent& e){
    if (e.button==0) {
        if(masterLoopRecorded==true){
            for (int i=0; i<8; i++) {
                recordedVideoPlayback[i].stop();
                recordedVideoPlayback[i].close();
            }
            
            file.removeFile("Slice1.mov");
        }
        
        vidRecorder->startRecording("Slice1.mov");
    }
    if (e.button==1) {
        startRecording(1);
    }
    if (e.button==2) {
        startRecording(2);
    }
    if (e.button==3) {
        startRecording(3);
    }
    if (e.button==4) {
        startRecording(4);
    }
    if (e.button==5) {
        startRecording(5);
    }
    if (e.button==6) {
        startRecording(6);
    }
    if (e.button==7) {
        startRecording(7);
    }
	cout << "BUTTON " << ofToString(e.button) << " PRESSED" << endl;
}
//--------------------------------------------------------------
void testApp::videoSaved(ofVideoSavedEventArgs& e){
	// the ofQTKitGrabber sends a message with the file name and any errors when the video is done recording
	if(e.error.empty()){
        if (sliceCount==0) {
            recordedVideoPlayback[sliceCount].loadMovie(e.videoPath,OF_QTKIT_DECODE_TEXTURE_ONLY);
            recordedVideoPlayback[sliceCount].setSynchronousSeeking(false);
            masterDuration=recordedVideoPlayback[sliceCount].getDuration();
            recordedVideoPlayback[sliceCount].play();
        }
        if (sliceCount!=0) {
            recordedVideoPlayback[sliceCount].loadMovie(e.videoPath,OF_QTKIT_DECODE_TEXTURE_ONLY);
            recordedVideoPlayback[sliceCount].setSynchronousSeeking(false);
            recordedVideoPlayback[sliceCount].setLoopState(OF_LOOP_NONE);
           
        }
	    
	}
	else {
		ofLogError("videoSavedEvent") << "Video save error: " << e.error;
	}
}


//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

