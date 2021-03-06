#include "ofApp.h"

bool ofApp::bAlignByPixel = false;

//--------------------------------------------------------------
void ofApp::setup(){

  // load settings
  Settings.load(ofToDataPath("settings.xml",true));
  
  bDebug = Settings.getValue("settings:debug", 0) > 0;
  if (bDebug) {
    ofSetLogLevel(OF_LOG_NOTICE);
  } else {
    ofSetLogLevel(OF_LOG_ERROR);
  }
  
  ofSetFullscreen(Settings.getValue("settings:fullscreen", 1) > 0);
  
  mustBeComment = false;
  
  int r,g,b;
  r = Settings.getValue("background:r", 51);
  g = Settings.getValue("background:g", 99);
  b = Settings.getValue("background:b", 59);
  bgShift = Settings.getValue("background:shift", 0.1);
  
  bgColor = ofColor(r,g,b);
  bgSat = bgColor.getSaturation();
  bgHue = bgColor.getHue();
  bgBright = bgColor.getBrightness();
  

  ofSetFrameRate(60);
  ofSetVerticalSync(true);
  ofBackground(bgColor);

  citeAmount = 0;
  bitmapRendering = true;
  
  // db settings, non-settable
  dbPath = ofToDataPath("zitate.csv", true);
  commentsPath = ofToDataPath("comments.csv", true);

  // cite statistics
  numCites = numComments = 0;
  
  // Prepare fonts
  loadFonts();
  
  setupMasterConnection();
  setupClientNetwork();
  
  scheduleDownload();
  scheduleReload();

  // -- Setup Timers
  setupTimers();
  
  align = UL2_TEXT_ALIGN_INVALID;
  
  // kick off the process
  startTimers();
  
}

//--------------------------------------------------------------
void ofApp::update(){
  masterConnectionUpdate();
  clientNetworkUpdate();
  updateAllTimers();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofHideCursor();
  
  ofBackground(bgColor);

  if (currentCitation != NULL) {
    
    ofSetColor(225);
    
    
    if (type.bIsRunning || waitForMeta.bIsRunning || showMeta.bIsRunning || waitRewind.bIsRunning || rewind.bIsRunning) {

      if (currentCitation->isComment()) {
        ofBackground(33, 102, 137);
      }
      
      if(currentCitation->body.size() <= Settings.getValue("settings:use-large-from", -1)) {
        mainFaceLarge.drawString(currentCitation->body, 50, Settings.getValue("settings:fonts:main-large:line-height", 100), ofGetWidth()-100, 500, align, citeAmount);
      } else {
        mainFace.drawString(currentCitation->body, 50, Settings.getValue("settings:fonts:main:line-height", 100) + 10, ofGetWidth()-100, 500, align, citeAmount);
      }
      
    }
    
    if (showMeta.bIsRunning) {
      
      citeNameFace.drawString(currentCitation->author ,50, ofGetHeight() - 4*34 - 50, ofGetWidth()-100, 4*34, UL2_TEXT_ALIGN_V_TOP|UL2_TEXT_ALIGN_RIGHT);
      
      // affilition[,  year]
      // reason
      string meta = "";
      
      if (currentCitation->affiliation.length() > 0) {
        meta += currentCitation->affiliation;
      }
      
      if (currentCitation->year.length() > 0) {
        meta += (meta.length() == 0 ? "" : ", ") + currentCitation->year;
      }
      
      if (currentCitation->reason.length() > 0 && currentCitation->reason != "comment") {
        meta += (meta.length() == 0 ? "" : "\n") + currentCitation->reason;
      }
      
      citeMetaFace.drawString(meta, 50, ofGetHeight()- 4*34 + 5 , ofGetWidth()-105, 3*34, UL2_TEXT_ALIGN_V_TOP|UL2_TEXT_ALIGN_RIGHT);
    }
  } else {
    if (bDebug) {
      ofBackground(255, 0, 0);
      mainFace.drawString("Debug: No citation found!", 50, 100);
    }
  }
  
  showTimersDebug();
  
  if (bDebug) {
    ofDrawBitmapString(ofToString(ofGetFrameRate(),2), ofGetWidth() - 50, 25);
    ofDrawBitmapString(bAlignByPixel ? "Aligned by pixel" : "Not aligned by pixel", ofGetWidth() - 200, 40);
  }
  
}

