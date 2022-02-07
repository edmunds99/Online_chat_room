// SocketClient.cpp: 实现文件
//

#include "pch.h"
#include "USocketClient.h"
#include "loginDlg.h"
#include "regGlg.h"
#include "chatcDlg.h"
#include "fileProDlg.h"
#include "resource.h"
// SocketClient
CString USocketClient::lpath=_T("");

void USocketClient::OnReceive(int nErrorCode)
{
	char sz[BUFTMP] = { 0 };
	int r=Receive(sz,BUFTMP);
	if (r > 0)
	{
		Lock(true);
		if (r + iend >=BUFSIZE)
		{
			if (iend - ibeg + r >= BUFSIZE)
			{
				bufsize *= 2;
			}
			char *tmp = new char[bufsize];
			int len = iend - ibeg;
			memcpy(tmp,mbuf+ibeg, len);
			ibeg = 0;
			iend = len;
			delete[]mbuf;
			mbuf = tmp;
		}
		memcpy(mbuf+iend, sz, r);
		iend += r;
		Lock(false);
		parsedata();
	}
	else
	{

	}
}

CString USocketClient::getaddr(sockaddr_in addr)
{
	wchar_t sz[256];
	InetNtopW(AF_INET, &addr.sin_addr, sz, 256);
	CString cs = sz;
	return cs;
}

bool USocketClient::startsvr(short port)
{
	bool bl = Create(port, SOCK_STREAM); //创建监听套接字
	if (bl)
	{
		bl = Listen();//开始监听
		if (!bl)
		{
			AfxMessageBox(_T("监听失败"));
			return false;
		}
		bl = AsyncSelect(FD_ACCEPT | FD_CLOSE);
	}
	else
	{
		AfxMessageBox(_T("创建套接字失败"));
		return false;
	}
}

void USocketClient::init()
{
	WSADATA wa;
	if (WSAStartup(MAKEWORD(2, 2), &wa) != 0)
	{
		AfxMessageBox(_T("sock初始化失败！"));
	}
}




bool USocketClient::connectsvr()
{
	TCHAR sz[1024];
	CString file = getpath() + _T("svr.ini"); //配置读取文件
	bool bl = GetPrivateProfileStringW(_T("服务"), _T("ip"), _T("127.0.0.1"), sz, 1024, file);
	mip = sz;
	bl = GetPrivateProfileStringW(_T("服务"), _T("端口"), _T("8888"), sz, 1024, file);
	mport = _wtoi(sz);

	bl = Connect(mip, mport);
	return bl;
}

void USocketClient::OnSend(int nErrorCode)
{
	
}
void USocketClient::OnConnect(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		state = 1;
		loginDlg::Onmsg(_T("网络#连接成功"));


		int nBuf = 1024*20;
		int nBufLen = sizeof(nBuf);
		int nRe = setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nBuf, nBufLen);
		
		//检查缓冲区是否设置成功
		 nRe = getsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nBuf, &nBufLen);


		 nBuf = 1024 * 1000;
		 nBufLen = sizeof(nBuf);
		 nRe = setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nBuf, nBufLen);
	}
	else
	{
		state = 0;
		Sleep(50);
		connectsvr();
	}
	
}
void USocketClient::OnClose(int nErrorCode)
{
	clearfile(NULL);
	state = 0;
	Close();
	Create();
	connectsvr();
	loginDlg::Onmsg(_T("网络#断线"));
	regGlg::Onmsg(_T("网络#断线"));
	CchatcDlg::Onmsg(_T("网络#断线"));
	
}


CString USocketClient::getpath()
{
	if (lpath.IsEmpty())
	{
		AfxGetModuleFileName(AfxGetInstanceHandle(), lpath);
		int n = lpath.ReverseFind('\\');
		if (n != -1)
		{
			lpath = lpath.Left(n + 1);
		}
	}
	return lpath;
}

void USocketClient::showinfo(CString cs)
{
	if (hwnd != NULL)
	{
		COleDateTime t = COleDateTime::GetCurrentTime();
		CString str = t.Format(_T("%Y-%m-%d %H:%M:%S"));
		CString res;
		res.Format(_T("[%s] %s"),str,cs);
		messages.push_back(res);
		PostMessage(hwnd, UMSG,0,0);
	}
}

int USocketClient::sendcmd(int cmd, CString cs)
{
	string str = UnicodeToAscii(cs.GetBuffer()).data();
	int r = sendcmd(cmd, str.c_str(), str.size());
	return r;
}

int USocketClient::sendcmd(int cmd, const char*pt, int len, char*buf)
{
	if (m_hSocket != INVALID_SOCKET)
	{
		char *psnd = NULL;
		if (buf != NULL)
		{
			psnd = buf;
		}
		else
		{
			psnd=new char[len + 10];
		}
		int l = 2;
		psnd[0] = 0x0a;		//消息开始标识
		psnd[1] = 0xee;
		int i = htonl(cmd);		//消息类型

		memcpy(psnd + l, &i, sizeof(i));
		l += sizeof(i);
		l += 4;

		memcpy(psnd + l, pt, len);	//数据
		l += len;

		int num = htonl(l - 10);
		memcpy(psnd + 6, &num, sizeof(num));	//数据长度
		int r = Send(psnd, l, 0);
		if (buf == NULL)
		{
			delete[]psnd;
		}
		return r;
	}
	return -1;
}

CString USocketClient::getstr(vector<CString>&vec, CString dep /*= _T("\r\n")*/)
{
	CString res = _T("");
	for (int i = 0;i < vec.size();i++)
	{
		res += vec[i];
		res += ',';
	}
	res += dep;
	return res;
}

CString USocketClient::getstr(vector<vector<CString>>&vec, CString dep /*= _T("\r\n")*/)
{
	CString res = _T("");
	CString cs;
	for (int i = 0;i < vec.size();i++)
	{
		cs = getstr(vec[i], dep);
		res += cs;
	}
	return res;
}

DWORD WINAPI USocketClient::thsendfile(LPVOID lp)
{
	UFile*tmp = (UFile*)lp;
	if (tmp == NULL)
	{
		return 0;
	}
	CString cs;
	int n = tmp->init(false);
	tmp->bsend = 1;
	
	fileProDlg *pdlg = new fileProDlg();
	tmp->pDlg = pdlg;
	pdlg->mfile = tmp;
	tmp->st = GetTickCount64();
	CchatcDlg::Inst->SendMessage(UMSG, (WPARAM)tmp);
		if (n == 0)
		{
			cs.Format(_T("打开文件%s失败，发送失败!"), tmp->path);
			AfxMessageBox(cs);
			return 0;
		}
		else
		{
			USocketClient *client = (USocketClient*)(tmp->pclient);
			char sz[4096] = { 0 };
			sz[0] = 0x0a;
			sz[1] = 0xee;
			n = FILE;
			n = htonl(n);
			memcpy(sz + 2, &n, sizeof(n));

			sz[DATAPOS] = 2;

			string str = UnicodeToAscii(tmp->getstr().GetBuffer());
			memcpy(sz + DATAPOS + 1, str.c_str(), str.size());

			int num = 0;
			int r = 0;
			int rsize = 4000 - HEADLEN;

			int nsize = 0;
			int n1;
			int ssize = 0;
			float f;
			float f1;
			
			
			while (num < tmp->size&&tmp->bsend==1)
			{
				n = tmp->file.Read(sz + HEADLEN + DATAPOS, rsize);
				if (n <= 0)
				{
					continue;
				}

				n1 = htonl(n + HEADLEN);
				memcpy(sz + 6, &n1, sizeof(n1));

				ssize = HEADLEN + n + DATAPOS;

				if (n <= 0)
				{
					break;
				}
				nsize = 0;
			    while (nsize < ssize)
				{
					r = client->Send(sz, ssize);
					if (r > 0)
					{
						nsize += r;
					}
				
				}
				num += n;
				tmp->rsize = num;
			
			}
			if (tmp->bsend == 1)
			{
				tmp->bsend = 0;
				sz[DATAPOS] = 4;
				n1 = htonl(HEADLEN);
				memcpy(sz + 6, &n1, sizeof(n1));

				client->Send(sz, HEADLEN+DATAPOS);
			}
			tmp->mth = NULL;
	
		}
}

void USocketClient::OnFile(int cmd, char *data, int len)
{
	if (len < HEADLEN)
	{
		return;
	}
	CString str;
	vector<CString>vec;
	CString cs = AsciiToUnicode(data + 1).data();
	data[HEADLEN - 1] = 0;
	int s = data[0];
	USocketClient::readwords(cs, ',', vec);
	if (vec.size() >= 4)
	{
		if (muser.act == vec[0])
		{
			if (s == 1)
			{
				UFile *tmp = findfile(vec[2]);
				if (tmp != NULL)
				{
					clearfile(tmp);
				}
				tmp = new UFile;
				tmp->sender = vec[0];
				tmp->recver = vec[1];
				tmp->path = vec[2];
				int i = tmp->path.ReverseFind('\\');
				if (i != -1)
				{
					tmp->name = tmp->path.Mid(i + 1);
				}
				tmp->pclient = this;
				tmp->size = _wtoi(vec[3]);
				files.push_back(tmp);
				tmp->mth=CreateThread(NULL, 0, thsendfile, (LPVOID)tmp, 0, NULL);

			}
			else if (s == 0)
			{
				str.Format(_T("%s接收文件 %s 失败"), vec[1], vec[2]);
				AfxMessageBox(str);
			}
			else if (s == 3)
			{
				str.Format(_T("%s 拒绝接收 %s"), vec[1], vec[2]);
				AfxMessageBox(str);
			}
			else if (s == 5)
			{
				UFile *tmp = findfile(vec[2]);
				if (tmp != NULL && tmp->bsend==1)
				{
					tmp->bsend = 5;
				}
			}
			else if (s == 10)
			{
		
				UFile *tmp = findfile(vec[2]);
				if (tmp != NULL && tmp->bsend==1)
				{
					tmp->bsend = s;
				}
			}
			else if (s == 7 || s == 6)
			{
				UFile *tmp = findfile(vec[2]);
				if (tmp != NULL && tmp->bsend==1)
				{
					tmp->bsend = s;
				}
			}
		}
		else  if (muser.act == vec[1])
		{

			if (s == 1)
			{
				str.Format(_T("接收%s 发送的文件 %s(%.3fkb)？"), vec[0], vec[2], _wtof(vec[3]) / 1000.0);
				if (AfxMessageBox(str, MB_YESNO) == IDYES)
				{
					UFile *tmp = findfile(vec[2]);
					if (tmp != NULL)
					{
						clearfile(tmp);
					}

					tmp = new UFile;
					tmp->sender = vec[0];
					tmp->recver = vec[1];
					int i = vec[2].ReverseFind('\\');
					if (i != -1)
					{
						tmp->name = vec[2].Mid(i + 1);
					}
					tmp->path = getpath() + tmp->name;
					tmp->pclient = this;
					tmp->size = _wtoi(vec[3]);
					tmp->st = GetTickCount64();
					data[0] = tmp->init(true);
					fileProDlg *pdlg = new fileProDlg();
					tmp->pDlg = pdlg;
					pdlg->mfile = tmp;
					pdlg->brec = true;
					tmp->bsend = 1;
					files.push_back(tmp);
					CchatcDlg::Inst->SendMessage(UMSG,(WPARAM)tmp);
				}
				else
				{
					data[0] = 3;
				}
				sendcmd(cmd, data, len);
			}
			else if (s == 10)
			{
			
				UFile *tmp = findfile(vec[2]);
				if (tmp != NULL)
				{
					tmp->bsend = 10;
				}
			}
			else if (s == 4)
			{
				
				UFile *tmp = findfile(vec[2]);
				if (tmp != NULL)
				{
					tmp->bsend = 4;
				}
			}
			else if (s == 2)
			{
				UFile *tmp = findfile(vec[2]);
				if (tmp != NULL)
				{
					int num = len - HEADLEN;
					if (num > 0)
					{
						tmp->rsize += num;
						tmp->file.Write(data + HEADLEN, num);
						
						if (tmp->rsize >= tmp->size)
						{
							tmp->bsend = 4;
						}
					}
					else
					{
						AfxMessageBox(_T("err"));
					}
				}
			}
		}

	}
	else
	{
		AfxMessageBox(_T("服务返回文件数据错误"));
	}
}

void USocketClient::Lock(bool bl)
{
	return;
	if (bl)
	{
		EnterCriticalSection(&mcs);
	}
	else
	{
		LeaveCriticalSection(&mcs);
	}
}
int USocketClient::getint(char *sz)
{
	char arr[4] = { 0 };
	memcpy(arr, sz, 4);
	int *pn = (int *)arr;
	int n = *pn;
	n=ntohl(n);
	return n;
}

void USocketClient::readfile(CString fpath, CString &cs) //读文件
{
	cs = _T("");
	CFile file;
	int res = file.Open(fpath, CFile::modeRead);
	if (!res)
	{
		return;
	}
	int len = file.GetLength();
	char *arr = new char[len + 1];  //分配内存
	len = file.Read(arr, len);    //读文件
	arr[len] = 0;
	cs = AsciiToUnicode(arr).data();
	file.Close();
	delete[]arr;
}

void USocketClient::readwords(CString cs, char c, vector<CString>&vec)
{
	vec.clear();
	cs = cs.Trim();
	
	if (cs.IsEmpty())
	{
		return;
	}
	int len = cs.GetLength();
	if (cs[len - 1] != c)
	{
		cs += c;
	}
	int pos1 = 0;
	int pos2 = cs.Find(c, pos1); //查找c位置
	CString str;
	while (pos2 != -1)
	{
		str = cs.Mid(pos1, pos2 - pos1);	//提取子字符串
		str = str.Trim();
		vec.push_back(str);

		pos1 = pos2 + 1;
		pos2 = cs.Find(c, pos1); //查找c位置
	}
}
void USocketClient::process(int cmd, CString cs)
{
	vector< vector<CString>>data;
	vector<CString>vec;
	readwords(cs, ',', vec);
	CString res = _T("");
	CString str;
	switch (cmd)
	{
	case  REG:
		regGlg::Onmsg(cs);
		break;
	case  LOGIN:
		loginDlg::Onmsg(cs);
		break;
	case  USERS:
		CchatcDlg::Onmsg(cs);
		break;
	case  CHAT:
		CchatcDlg::Onmsg(cs);
		break;
	case  CHAT2:
		CchatcDlg::Onmsg(cs);
		break;
	case  FRIFEND:
		CchatcDlg::Onmsg(cs);
		break;
	case  FRIFENDS:
		CchatcDlg::Onmsg(cs);
		break;
	default:
		CchatcDlg::Onmsg(cs);
		break;
	}
}
void USocketClient::parsedata()
{
	Lock(true);
	int datasize = iend - ibeg;			//数据长度
	CString cs;
	CString msg;
	int cmd;
	int nsize;
	while (datasize >= 10)
	{
		unsigned char c1 = mbuf[ibeg];
		unsigned char c2 = mbuf[ibeg + 1];
		if (mbuf[ibeg] == 0x0a)		//消息开始标识
		{
			if (c2 == 0xee)		//消息开始
			{
				cmd = getint(mbuf+ibeg+2);
				nsize = getint(mbuf + ibeg + 6);	
				if (datasize - 10 < nsize)
				{
					break;
				}
				char tmp[4096] = { 0 };
				ibeg += 10;
				memcpy(tmp, mbuf + ibeg, nsize);  //数据
				ibeg += nsize;        //数据开始位置更新
				datasize = iend - ibeg;
				msg = AsciiToUnicode(tmp).data();
				if (cmd == FILE)
				{
					OnFile(cmd, tmp, nsize);
				}
				else
				{

					process(cmd, msg);	//消息处理
				}
			}
		}
		else
		{
			int i;
			for (i = ibeg; i < iend; i++)	//消息错误
			{
				if (mbuf[i] == 0x0a)
				{
					if (i + 1 < iend)
					{
						if (mbuf[i + 1] == 0xee)
						{
							break;
						}
					}

				}
			}
			if (i == iend && mbuf[iend - 1] != 0x0a)	//无消息开始标识
			{
				ibeg = 0;
				iend = 0;

			}
			else
			{
				if (i == iend)i--;	//消息开始标识
				ibeg = i;
			}
			datasize = iend - ibeg;
		}
	}
	Lock(false);
}
DWORD WINAPI USocketClient::threceive(LPVOID p)
{
	USocketClient*psock = (USocketClient*)p;
	if (psock == NULL)
	{
		return 0;
	}
	while (psock->state!=0)
	{
		psock->parsedata();
	}
	return 0;
}
string USocketClient::Utf8ToAscii(const string& str)
{
	return UnicodeToAscii(Utf8ToUnicode(str));
}
string USocketClient::AsciiToUtf8(const string& str)
{
	return UnicodeToUtf8(AsciiToUnicode(str));
}
wstring USocketClient::Utf8ToUnicode(const string& str)
{
	// 预算-缓冲区中宽字节的长度    
	int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	// 给指向缓冲区的指针变量分配内存    
	wchar_t *pUnicode = (wchar_t*)malloc(sizeof(wchar_t)*unicodeLen);
	// 开始向缓冲区转换字节    
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, pUnicode, unicodeLen);
	wstring ret_str = pUnicode;
	free(pUnicode);
	return ret_str;
}
string USocketClient::UnicodeToUtf8(const wstring& wstr)
{
	// 预算-缓冲区中多字节的长度  
	int ansiiLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	// 给指向缓冲区的指针变量分配内存  
	char *pAssii = (char*)malloc(sizeof(char)*ansiiLen);
	// 开始向缓冲区转换字节  
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, pAssii, ansiiLen, nullptr, nullptr);
	string ret_str = pAssii;
	free(pAssii);
	return ret_str;
}
wstring USocketClient::AsciiToUnicode(const string& str)
{
	// 预算-缓冲区中宽字节的长度  
	int unicodeLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	// 给指向缓冲区的指针变量分配内存  
	wchar_t *pUnicode = (wchar_t*)malloc(sizeof(wchar_t)*unicodeLen);
	// 开始向缓冲区转换字节  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, pUnicode, unicodeLen);
	wstring ret_str = pUnicode;
	free(pUnicode);
	return ret_str;
}
string USocketClient::UnicodeToAscii(const wstring& wstr)
{
	// 预算-缓冲区中多字节的长度    
	int ansiiLen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	// 给指向缓冲区的指针变量分配内存    
	char *pAssii = (char*)malloc(sizeof(char)*ansiiLen);
	// 开始向缓冲区转换字节    
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pAssii, ansiiLen, nullptr, nullptr);
	string ret_str = pAssii;
	free(pAssii);
	return ret_str;
}
UFile* USocketClient::findfile(CString fn)
{
	int i = fn.ReverseFind('\\');
	if (i != -1)
	{
		fn = fn.Mid(i + 1);
	}
	for (int i = 0;i < files.size();i++)
	{
		if (fn == files[i]->name)
		{
			return files[i];
		}
	}
	return NULL;
}
bool USocketClient::clearfile(UFile*pfile)
{
	if (pfile == NULL)
	{
		for (int i = 0;i < files.size();i++)
		{
			if (files[i] != NULL)
			{
				files[i]->bsend = -1;
				files[i]->clear();
				delete files[i];
			}
		}
		files.clear();
		return true;
	}
	else
	{
		UFile *tmp = NULL;
		for (vector<UFile*>::iterator it = files.begin();it != files.end();it++)
		{
			tmp = *it;
			if (tmp == pfile)
			{
				files.erase(it);
				tmp->clear();
				delete tmp;
				return true;
			}
		}
		return false;
	}


}

// SocketClient 成员函数
