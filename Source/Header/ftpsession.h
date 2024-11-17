//
// Created by ngoqh on 11/11/2024.
//

#ifndef FTPSESSION_H_INCLUDED
#define FTPSESSION_H_INCLUDED
#include <cstdint>
#include <vector>
#include "tcpserversocket.h"
#include "ftpserverconfig.h"
#include "session.h"

#define STATUS_INIT 0
#define STATUS_USER 1
#define STATUS_PASS 2

#define USER "USER"
#define PASS "PASS"
#define MKD "MKD"
#define CWD "CWD"
#define RMD "RMD"
#define PORT "PORT"
#define RETR "RETR"
#define DELE "DELE"

#ifndef QUIT
#define QUIT "QUIT"
#endif

struct FTPSessionInfo {
    uint8_t status;
    string username;
    string password;
};

class FTPSession : public Session {
    FTPSessionInfo *sessionInfo;
    string response;

    void reset() override;

    FTPServerConfig *getServerConfig() const {
        return (FTPServerConfig *) conf;
    };

public:
    FTPSession(const TcpSocket &slave, ServerConfig *conf);

    ~FTPSession();

    const string &getResponse() const {
        return response;
    };

    void doUSER(string cmd_argv[], int cmd_argc);

    void doPASS(string cmd_argv[], int cmd_argc);

    void doMKD(string cmd_argv[], int cmd_argc);

    void doCWD(string cmd_argv[], int cmd_argc);

    void doRMD(string cmd_argv[], int cmd_argc);

    void doPORT(string cmd_argv[], int cmd_argc);

    void doRETR(string cmd_argv[], int cmd_argc);

    void doDELE(string cmd_argv[], int cmd_argc);

    void doQUIT(string cmd_argv[], int cmd_argc);

    void doUnknown(string cmd_argv[], int cmd_argc) override;
};

#endif //FTPSESSION_H
