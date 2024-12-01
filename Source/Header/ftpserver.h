//
// Created by ngoqh on 11/11/2024.
//

#ifndef FTPSERVER_H
#define FTPSERVER_H
#include "ftpserverconfig.h"
#include "ftpsession.h"
#include "tcpserver.h"

class FTPServer : public TCPServer
{
public:
    FTPServer(unsigned short port = 21);

    ~FTPServer();

    bool configServer() override;

protected:
    void cleanServer();

    FTPServerConfig* getServerConfig()
    {
        return (FTPServerConfig*)conf;
    }

    bool loadServerConfig(const string& confFileName) override;

    void startNewSession(TcpSocket slave) override;

    void initCmd() override;

private:
    int readCmd(TcpSocket& slave, string& cmdLine);

    unsigned short parseCmd(const string& cmdLine, string cmd_argv[], int& cmd_argc);
};
#endif // FTPSERVER_H
