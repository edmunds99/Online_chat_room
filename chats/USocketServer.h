#pragma once
#include <iostream>
#include <vector>
using namespace std;
// SocketClient 命令目标
#define BUFSIZE 51200
#define BUFTMP 51200
#define  UMSG WM_USER+1
#include "CMysql.h"

class USocketServer : public CAsyncSocket
{
public:
	vector<USocketServer*>clients;
	vector<CString>messages;
	
	CRITICAL_SECTION mcsclients;
	CRITICAL_SECTION mcsbuf;
	char *mbuf;
	int bufsize;
	int ibeg;
	int iend;
	int state;
	HANDLE hrecv;
	sockaddr_in IpAddr;
	CString mip;
	short mport;
	USocketServer*psvr;
	CString lpath;
	HWND hwnd;
	CMysql *mdb;
	User muser;
	enum NMSG { REG, LOGIN, FRIFEND, CHAT, CHAT2, USERS, FRIFENDS, FILE,FILEDATA};
	vector<UFile*>files;
	int num;
	bool bs;
	USocketServer::USocketServer()
	{
		bs = false;
		bufsize = BUFSIZE;
		mbuf = new char[bufsize];
		ibeg = 0;
		iend = 0;
		state = 0;
		InitializeCriticalSection(&mcsclients);
		InitializeCriticalSection(&mcsbuf);
		hrecv = NULL;
		psvr = NULL;
		lpath = _T("");
		mip = _T("127.0.0.1");
		mport = 7777;
		hwnd = NULL;
	
		
	}
	
	virtual USocketServer::~USocketServer()
	{
		Close();
		state = 0;
		Sleep(100);
		if (hrecv != NULL)
		{
			TerminateThread(hrecv, 0);
			hrecv = NULL;
		}
	}
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	void OnClientOffline(USocketServer*sock);
	void LockClient(bool bl);
	void LockBuf(bool bl);
	static DWORD WINAPI threceive(LPVOID p);
	void parsedata();
	int getint(char *sz);
	static string Utf8ToAscii(const string& str);  //转码
	static string AsciiToUtf8(const string& str);
	static wstring AsciiToUnicode(const string& str);
	static string UnicodeToAscii(const wstring& wstr);
	static string UnicodeToUtf8(const wstring& wstr);
	static wstring Utf8ToUnicode(const string& str);
	void readfile(CString fpath, CString &cs); //读文件
	void readwords(CString cs, char c, vector<CString>&vec);
	void process(int cmd, CString cs);
	
	void addclient(USocketServer *sock);
	CString getaddr(sockaddr_in addr);
	bool startsvr(short port);
	static void init();
	void closesvr();
	bool connectsvr();
	void del(USocketServer*sock);
	CString getpath();
	void showinfo(CString cs);
	void initdb();
	int sendcmd(int cmd, CString cs);
	int sendcmd(int cmd, const char*pt, int len);
	CString getstr(vector<CString>&vec, CString dep = _T("\r\n"));
	CString getstr(vector<vector<CString>>&vec, CString dep = _T("\r\n"));
	CString getusers();
	CString getfriends(int id);
	void broadmsg(int cmd, CString cs);
	void senddata();
	void getdata();
	void onchat_11(CString cs);
	void onchat_1n(CString cs);
	void onfriend(vector<CString>vec);
	USocketServer *findclient(CString n);
	UFile*findfile(CString fn);
	bool clearfile(UFile*pfile);
	void OnFile(int cmd, char *data, int len);
	void  senddata(int cmd, char *data, int len);
};


