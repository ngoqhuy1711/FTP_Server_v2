//
// Created by ngoqh on 11/11/2024.
//
#include "ftpsession.h"
#include <cstring>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "ftpsession.h"
#include "ftpserverconfig.h"


FTPSession::FTPSession(const TcpSocket &slave, ServerConfig *conf): Session(slave, conf) {
    this->sessionInfo = new FTPSessionInfo();
    this->quitSession = false;
    this->sessionInfo->status = STATUS_INIT;
}
void FTPSession::reset() {
    this->sessionInfo->status = STATUS_INIT;
    this->sessionInfo->fromAddress.clear();
    this->sessionInfo->toAddress.clear();
}
FTPSession::~FTPSession() {
    delete sessionInfo;
}
void FTPSession::doUSER(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doPASS(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doMKD(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doCWD(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doRMD(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doPORT(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doRETR(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doSTOR(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doQUIT(string cmd_argv[], int cmd_argc) {

}
void FTPSession::doUnknown(string cmd_argv[], int cmd_argc) {

}

