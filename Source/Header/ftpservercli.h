//
// Created by ngoqh on 11/11/2024.
//

#ifndef FTPCLI_H_INCLUDED
#define FTPCLI_H_INCLUDED
#include "cli.h"
#include "ftpserver.h"
#ifdef _WIN32
#include <windows.h>
#endif

class FTPServerCLI : public CmdLineInterface {
private:
    FTPServer *server;

public:
    FTPServerCLI();

    ~FTPServerCLI();

protected:
    virtual void initCmd() override;

private:
    void doStart(string cmd_argv[], int cmd_argc);

    void doStop(string cmd_argv[], int cmd_argc);

    void doStatus(string cmd_argv[], int cmd_argc);

    void doRestart(string cmd_argv[], int cmd_argc);

    void doHelp(string cmd_argv[], int cmd_argc);
};

#endif //FTPCLI_H
