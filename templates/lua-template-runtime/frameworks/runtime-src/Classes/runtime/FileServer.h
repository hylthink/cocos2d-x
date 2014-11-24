/****************************************************************************
Copyright (c) 2013 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef  _FILE_SERVER__H_
#define  _FILE_SERVER__H_

#include "cocos2d.h"
#include "json/document.h"
#include "json/filestream.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "Protos.pb.h"
#include <string>

using namespace std;
using namespace cocos2d;

// header files for socket
#ifdef _WIN32
#include <io.h>
#include <WS2tcpip.h>

#define bzero(a, b) memset(a, 0, b);

#else
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#ifdef _WIN32
#define usleep(t) Sleep(t)
#else
#include <unistd.h>
#endif

class FileServer
{
    static FileServer *s_sharedFileServer;
public:
    static FileServer* getShareInstance()
    {
        if (s_sharedFileServer == nullptr)
        {
            s_sharedFileServer = new FileServer();
        }
        return s_sharedFileServer;
    }
    bool listenOnTCP(int port);
    void stop();
    
    void readResFileFinfo();
    void addResFileInfo(const char* filename,uint64_t u64);
    void removeResFileInfo(const char *filename);
    rapidjson::Document* getFileCfgJson() { return &_filecfgjson; }
    string getWritePath() { return _writePath; }
    string getTransingFileName();
    void setTransingFileName(const string& filename);
protected:
    FileServer();
    ~FileServer();
private:
    void loopReceiveFile();
    void loopWriteFile();
    void loopResponse();
    void addResponse(int fd, string filename,int errortype,int errornum);
    enum PROTONUM
    {
        FILEPROTO = 1,
        FILESENDCOMPLETE = 2,
        DIRPROTO = 3,
        DIRSENDCOMPLETE = 4
    };
    
    struct RecvBufStruct
    {
        runtime::FileSendProtos fileProto;
        std::string contentBuf;
        int fd;
    };
    
    struct ResponseStruct
    {
        runtime::FileSendComplete fileResponseProto;
        int fd;
    };
    
    // file descriptor: socket, console, etc.
    int _listenfd;
    int _maxfd;
    std::vector<int> _fds;
    std::thread _responseThread;
    std::thread _receiveThread;
    std::thread _writeThread;
    fd_set _read_set;
    bool _running;
    bool _endThread;
    char *_protoBuf;
    
    std::list<RecvBufStruct> _recvBufList;
    std::list<ResponseStruct> _responseBufList;
    std::mutex _recvBufListMutex;
    std::mutex _responseBufListMutex;
    
    rapidjson::Document _filecfgjson;
    
    string _strFileName;
    std::mutex _fileNameMutex;
    
    string _recvErrorFile;
    string _writeErrorFile;
    
    string _writePath;
};

bool createDir(const char *sPathName);

#endif // _FILE_SERVER__H_

