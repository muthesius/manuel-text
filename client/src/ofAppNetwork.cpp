//
//  ofAppNetwork.cpp
//  CiteClient
//
//  Created by jens on 20.03.15.
//
//

#include "ofApp.h"


void ofApp::setupNetwork() {
  setupClientNetwork();
  setupMasterConnection();
}

void ofApp::setupClientNetwork() {
  clientNet.SetEnableBroadcast(true);
  clientNet.SetNonBlocking(true);
  clientNet.SetReuseAddress(true);
  clientNet.Bind(3331);
  clientNet.Create();
}

void ofApp::broadCastClients(string msg) {
  // TODO: Use IP address from a settings file, or try to get from system call
  clientNet.Connect("255.255.255.255", 3331);
  clientNet.SendAll(msg.c_str(), msg.size());
  clientNet.Close();
}

void ofApp::clientNetworkUpdate() {
  int buff_size = clientNet.GetReceiveBufferSize();
  if (buff_size > 0) {
    char* buffer = (char*) malloc(buff_size * sizeof(char));
    clientNet.Receive(buffer, buff_size);
    string msg = string(buffer);
    free(buffer);
    ofLog() << "Client Network recieved: " << msg;
  }
}

void ofApp::setupMasterConnection() {
  masterConnection.SetEnableBroadcast(true);
  masterConnection.SetNonBlocking(true);
  clientNet.SetReuseAddress(true);
  masterConnection.Bind(3332);
  masterConnection.Create();
}

void ofApp::masterConnectionUpdate() {
  
}