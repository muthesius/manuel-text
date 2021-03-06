#pragma once

#include "ofMain.h"
#include "ofxCsv.h"
#include "ofxSimpleTimer.h"
#include "ofxTrueTypeFontUL2.h"
#include "ofxIO.h"
#include "ofxNetwork.h"
#include "ofxXmlSettings.h"
#include "Citation.h"

// for ofxCsv:
using namespace wng;
using namespace std;
using namespace ofx::IO;

class ofApp : public ofBaseApp{

public:
  void setup();
  void update();
  void draw();

  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);

  bool bDebug;
  int citeAmount;
  static bool bAlignByPixel;
  
  ofxXmlSettings Settings;

  ofxUDPManager clientNet;
  ofxUDPManager masterConnection;
  
  void setupNetwork();
  
  void setupClientNetwork();
  void clientNetworkUpdate();
  void broadCastClients(string msg);

  void setupMasterConnection();
  void masterConnectionUpdate();
  
  // We want to use Unicode features to be safe and
  // get better text layout with harfbuzz
  ofxTrueTypeFontUL2 mainFace, mainFaceLarge, citeNameFace, citeMetaFace;
  void loadFonts();

  string cite_partial;

  Citation *currentCitation;
  
  bool bitmapRendering;
  int align;

  ofxSimpleTimer idle, type, waitForMeta, showMeta, waitRewind, rewind;
  vector<ofxSimpleTimer*> timers;
  
  void idleTimerStartHandler(int &args);
  void idleTimerCompleteHandler(int &args);
  
  void typeTimerStartHandler(int &args);
  void typeTimerCompleteHandler(int &args);

  void waitForMetaTimerStartHandler(int &args);
  void waitForMetaTimerCompleteHandler(int &args);
  
  void showMetaTimerStartHandler(int &args);
  void showMetaTimerCompleteHandler(int &args);
  
  void waitRewindTimerStartHandler(int &args);
  void waitRewindTimerCompleteHandler(int &args);
  
  void rewindTimerStartHandler(int &args);
  void rewindTimerCompleteHandler(int &args);

  void setupTimers();
  void startTimers();
  void updateAllTimers();
  void showTimersDebug();

  ofColor bgColor;
  float bgSat,bgHue,bgBright;
  double bgShift;
  void newBackground();

  // Citation DB
  ofxCsv db, commentsDb;
  string dbPath, commentsPath;
  void loadDB();
  bool doReload, doDownloadCites, doDownloadComments;
  
  list<int> citations,comments;
  int numCites,numComments;
  
  void notifyCiting(Citation* c);
  
  void setupDB();
  
  void nextCitation();
  
  bool popCitation(int id);
  bool popComment(int id);
  
  bool downloadCites();
  bool downloadComments();
  
  void scheduleReload();
  void scheduleUserComment();
  void scheduleDownload();
  bool mustBeComment;
  
  void DumpCitationList();
  
};
