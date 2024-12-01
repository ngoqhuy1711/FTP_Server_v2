//
// Created by ngoqh on 11/11/2024.
//
#include "ftpsession.h"
#include <algorithm>
#include <cstring>
#include <direct.h>
#include <fstream>
#include <iostream>
#include <io.h>
#include <sys/stat.h>
#include <sstream>

#include "ftpserverconfig.h"

FTPSession::FTPSession(const TcpSocket& slave, ServerConfig* conf) : Session(slave, conf)
{
    this->sessionInfo = new FTPSessionInfo();
    this->quitSession = false;
    this->sessionInfo->status = STATUS_INIT;
}

void FTPSession::reset()
{
    this->sessionInfo->status = STATUS_INIT;
    this->sessionInfo->username.clear();
    this->sessionInfo->password.clear();
}

FTPSession::~FTPSession()
{
    delete sessionInfo;
}

void FTPSession::doUSER(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 2)
    {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    string username = cmd_argv[1];
    if (conf->isValidUser(cmd_argv[1]))
    {
        sessionInfo->username = username;
        sessionInfo->status = STATUS_USER;
        response = "331 Mật khẩu cần thiết.\r\n";
        slave.send(response.c_str(), response.length());
    }
    else
    {
        response = "530 Tên người dùng không hợp lệ.\r\n";
        slave.send(response.c_str(), response.length());
    }
}

void FTPSession::doPASS(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 2)
    {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status == STATUS_USER)
    {
        string password = cmd_argv[1];
        if (conf->authenticate(sessionInfo->username, password))
        {
            sessionInfo->status = STATUS_PASS;
            response = "230 Đăng nhập thành công.\r\n";
            slave.send(response.c_str(), response.length());
        }
        else
        {
            response = "530 Mật khẩu không hợp lệ.\r\n";
            slave.send(response.c_str(), response.length());
        }
    }
}

void FTPSession::doMKD(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 2)
    {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    string dir = cmd_argv[1];
    if (_mkdir(dir.c_str()) == 0)
    {
        response = "257 Tạo thư mục thành công.\r\n";
        slave.send(response.c_str(), response.length());
    }
    else
    {
        response = "550 Tạo thư mục thất bại.\r\n";
        slave.send(response.c_str(), response.length());
    }
}

void FTPSession::doCWD(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 2)
    {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    string dir = cmd_argv[1];
    if (_chdir(dir.c_str()) == 0)
    {
        response = "250 Đã thay đổi thư mục.\r\n";
        slave.send(response.c_str(), response.length());
    }
    else
    {
        response = "550 Thay đổi thư mục thất bại.\r\n";
        slave.send(response.c_str(), response.length());
    }
}

void FTPSession::doRMD(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 2)
    {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    string dir = cmd_argv[1];
    struct _finddata_t fileinfo;
    intptr_t handle = _findfirst((dir + "\\*").c_str(), &fileinfo);

    if (handle == -1)
    {
        response = "550 Thư mục không tồn tại.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }

    bool isEmpty = true;
    do
    {
        if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
        {
            isEmpty = false;
            break;
        }
    }
    while (_findnext(handle, &fileinfo) == 0);

    _findclose(handle);

    if (!isEmpty)
    {
        response = "550 Thư mục không rỗng.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }

    if (_rmdir(dir.c_str()) == 0)
    {
        response = "250 Xóa thư mục thành công.\r\n";
        slave.send(response.c_str(), response.length());
    }
    else
    {
        response = "550 Xóa thư mục thất bại.\r\n";
        slave.send(response.c_str(), response.length());
    }
}

void FTPSession::doPORT(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 2)
    {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    string portInfo = cmd_argv[1];
    std::replace(portInfo.begin(), portInfo.end(), ',', ' ');
    std::istringstream iss(portInfo);
    int h1, h2, h3, h4, p1, p2;
    iss >> h1 >> h2 >> h3 >> h4 >> p1 >> p2;
    string ip = std::to_string(h1) + "." + std::to_string(h2) + "." + std::to_string(h3) + "." + std::to_string(h4);
    unsigned short port = p1 * 256 + p2;
    sessionInfo->dataIpAddress = ip;
    sessionInfo->dataPort = port;
    response = "200 Mở cổng " + ip + ":" + std::to_string(port) + " thành công.\r\n";
    slave.send(response.c_str(), response.length());
}

void FTPSession::doRETR(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 2)
    {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    string file = cmd_argv[1];
    if (sessionInfo->dataIpAddress.empty() || sessionInfo->dataPort == 0)
    {
        response = "425 Không mở cổng dữ liệu.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    TcpSocket dataSocket;
    dataSocket.connect(sessionInfo->dataIpAddress, sessionInfo->dataPort);
    response = "150 Đang gửi file.\r\n";
    slave.send(response.c_str(), response.length());
    ifstream fileStream(file, ios::in | ios::binary);
    if (!fileStream.is_open())
    {
        response = "550 Không thể mở file.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }

    char buffer[1024];
    while (fileStream.read(buffer, sizeof(buffer)) || fileStream.gcount() > 0)
    {
        dataSocket.send(buffer, fileStream.gcount());
    }

    fileStream.close();
    dataSocket.close();

    response = "226 Đã gửi xong file.\r\n";
    slave.send(response.c_str(), response.length());
}

void FTPSession::doDELE(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 2)
    {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    string file = cmd_argv[1];
    if (remove(file.c_str()) == 0)
    {
        response = "250 Xóa file thành công.\r\n";
        slave.send(response.c_str(), response.length());
    }
    else
    {
        response = "550 Xóa file thất bại.\r\n";
        slave.send(response.c_str(), response.length());
    }
}

void FTPSession::doQUIT(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 1)
    {
        response = "501 Cần thêm tham số.";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.";
        slave.send(response.c_str(), response.length());
        return;
    }
    response = "211 Tạm biệt";
    slave.send(response.c_str(), response.length());
    quitSession = true;
}

void FTPSession::doSYST(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 1)
    {
        response = "501 Cần thêm tham số.";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.";
        slave.send(response.c_str(), response.length());
        return;
    }
    response = "215 Kiểu UNIX: L8\r\n";
    slave.send(response.c_str(), response.length());
}

void FTPSession::doFEAT(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 1)
    {
        response = "501 Cần thêm tham số.";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.";
        slave.send(response.c_str(), response.length());
        return;
    }
    response = "211 Các lệnh khả dụng:\r\n";
    response += "USER\r\n";
    response += "PASS\r\n";
    response += "MKD\r\n";
    response += "CWD\r\n";
    response += "RMD\r\n";
    response += "PORT\r\n";
    response += "RETR\r\n";
    response += "DELE\r\n";
    response += "QUIT\r\n";
    slave.send(response.c_str(), response.length());
}

void FTPSession::doPWD(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 1)
    {
        response = "501 Cần thêm tham số.";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.";
        slave.send(response.c_str(), response.length());
        return;
    }
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        response = "257 \"" + string(cwd) + "\"\r\n";
        slave.send(response.c_str(), response.length());
    }
    else
    {
        response = "550 Lỗi.\r\n";
        slave.send(response.c_str(), response.length());
    }
}

void FTPSession::doTYPE(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 2)
    {
        response = "501 Cần thêm tham số.";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (cmd_argv[1] == "I")
    {
        response = "200 Chuyển sang chế độ nhị phân.\r\n";
        slave.send(response.c_str(), response.length());
    }
    if (cmd_argv[1] == "A")
    {
        response = "200 Chuyển sang chế độ ASCII.\r\n";
        slave.send(response.c_str(), response.length());
    }
    else
    {
        response = "504 Không hỗ trợ chế độ này.\r\n";
        slave.send(response.c_str(), response.length());
    }
}

void FTPSession::doLIST(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 1)
    {
        response = "501 Cần thêm tham số.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->dataIpAddress.empty() || sessionInfo->dataPort == 0)
    {
        response = "425 Không mở cổng dữ liệu.\r\n";
        slave.send(response.c_str(), response.length());
        return;
    }
    TcpSocket dataSocket;
    dataSocket.connect(sessionInfo->dataIpAddress, sessionInfo->dataPort);
    response = "150 Đang gửi danh sách thư mục.\r\n";
    slave.send(response.c_str(), response.length());

    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        string searchPath = string(cwd) + "\\*";
        struct _finddata_t fileinfo;
        intptr_t handle = _findfirst(searchPath.c_str(), &fileinfo);

        if (handle == -1)
        {
            response = "550 Lỗi.\r\n";
            slave.send(response.c_str(), response.length());
            return;
        }

        do
        {
            string entry;
            if (fileinfo.attrib & _A_SUBDIR)
            {
                entry = "drwxr-xr-x 1 owner group " + to_string(fileinfo.size) + " Jan 01 00:00 " + fileinfo.name +
                    "\r\n";
            }
            else
            {
                entry = "-rw-r--r-- 1 owner group " + to_string(fileinfo.size) + " Jan 01 00:00 " + fileinfo.name +
                    "\r\n";
            }
            dataSocket.send(entry.c_str(), entry.length());
        }
        while (_findnext(handle, &fileinfo) == 0);

        _findclose(handle);
        response = "226 Đã gửi xong danh sách thư mục.\r\n";
        slave.send(response.c_str(), response.length());
    }
    else
    {
        response = "550 Lỗi khi lấy thư mục hiện tại.\r\n";
        slave.send(response.c_str(), response.length());
    }

    dataSocket.close();
}

void FTPSession::doCDUP(string cmd_argv[], int cmd_argc)
{
    if (cmd_argc < 1)
    {
        response = "501 Cần thêm tham số.";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (sessionInfo->status != STATUS_PASS)
    {
        response = "530 Cần đăng nhập trước.";
        slave.send(response.c_str(), response.length());
        return;
    }
    if (_chdir("..") == 0)
    {
        response = "250 Đã thay đổi thư mục.\r\n";
        slave.send(response.c_str(), response.length());
    }
    else
    {
        response = "550 Thay đổi thư mục thất bại.\r\n";
        slave.send(response.c_str(), response.length());
    }
}

void FTPSession::doUnknown(string cmd_argv[], int cmd_argc)
{
}
