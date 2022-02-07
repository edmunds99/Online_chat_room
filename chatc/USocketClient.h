#pragma once
#include <iostream>
#include <vector>
using namespace std;
// SocketClient 命令目标
#define BUFSIZE 10240
#define BUFTMP 4096
#define UMSG WM_USER+1
#define SUCC _T("succ")
#include "../../chats//chats/User.h"
class USocketClient : public CAsyncSocket
{
public:
	vector<CString>messages;
	CRITICAL_SECTION mcs;
	char *mbuf;
	int bufsize;
	int ibeg;
	int iend;
	int state;
	HANDLE hrecv;
	sockaddr_in IpAddr;
	CString mip;
	short mport;
	static CString lpath;
	HWND hwnd;


	enum NMSG { REG, LOGIN, FRIFEND, CHAT, CHAT2, USERS, FRIFENDS, FILE };
	vector<UFile*>files;
	User muser;
	USocketClient::USocketClient()
	{
		bufsize = BUFSIZE;
		mbuf = new char[bufsize];
		ibeg = 0;
		iend = 0;
		state = 0;
		InitializeCriticalSection(&mcs);
		hrecv = NULL;
		lpath = _T("");
		mip = _T("127.0.0.1");
		mport = 7777;
		hwnd = NULL;
		

		
		
	}

	virtual USocketClient::~USocketClient()
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
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	void Lock(bool bl);
	static DWORD WINAPI threceive(LPVOID p);
	void parsedata();
	int getint(char *sz);
	static string Utf8ToAscii(const string& str);  //转码
	static string AsciiToUtf8(const string& str);
	static wstring AsciiToUnicode(const string& str);
	static string UnicodeToAscii(const wstring& wstr);
	static string UnicodeToUtf8(const wstring& wstr);
	static wstring Utf8ToUnicode(const string& str);
	static void readfile(CString fpath, CString &cs); //读文件
	static void readwords(CString cs, char c, vector<CString>&vec);
	void process(int cmd, CString cs);
	CString getaddr(sockaddr_in addr);
	bool startsvr(short port);
	static void init();
	bool connectsvr();
	static CString getpath();
	void showinfo(CString cs);
	UFile*findfile(CString fn);
	bool clearfile(UFile*pfile);
	int sendcmd(int cmd, CString cs);
	int sendcmd(int cmd, const char*pt, int len,char*buf=NULL);
	static CString getstr(vector<CString>&vec, CString dep = _T("\r\n"));
	static CString getstr(vector<vector<CString>>&vec, CString dep = _T("\r\n"));
	static DWORD WINAPI thsendfile(LPVOID lp);
	void OnFile(int cmd, char *data, int len);
	void SendFile(CString cs)
	{
		
		vector<CString>vec;
		readwords(cs, ',', vec);
		if (vec.size() >= 4)
		{

			bool bs = isend();
			if (bs)
			{
				AfxMessageBox(_T("发送失败，正在发送"));
				return;
			}
			UFile *tmp = findfile(vec[2]);
			if (tmp != NULL)
			{
				clearfile(tmp);
				
			}
			char sz[1024] = { 0 };
			sz[0] = 1;
			string str = UnicodeToAscii(cs.GetBuffer());
			memcpy(sz + 1, str.c_str(), str.size());
			sendcmd(NMSG::FILE, sz, HEADLEN);
		}
		else
		{
			AfxMessageBox(_T("数据错误"));
		}
	}
	bool isend()
	{
		for (int i=0;i<files.size();i++)
		{
			if (files[i]->bsend == 1)
			{
				return true;
			}
		}
		return false;
	}
};


