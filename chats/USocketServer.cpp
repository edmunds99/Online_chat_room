// SocketClient.cpp: 实现文件
//

#include "pch.h"
#include "USocketServer.h"


// SocketClient


void USocketServer::OnReceive(int nErrorCode)
{
	char sz[10240] = { 0 };
	int r=Receive(sz, 10240);
	if (r > 0)
	{
	
		if (r + iend >=BUFSIZE)
		{
			if (r + iend - ibeg >= BUFSIZE)
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
		parsedata();
	}
	else
	{

	}
}
void USocketServer::addclient(USocketServer *sock)
{
	LockClient(true);
	clients.push_back(sock); //保存连接
	LockClient(false);
}

CString USocketServer::getaddr(sockaddr_in addr)
{
	wchar_t sz[256];
	InetNtopW(AF_INET, &addr.sin_addr, sz, 256);
	CString cs = sz;
	return cs;
}
void USocketServer::getdata()
{
	if (mdb != NULL)
	{
		if (mdb->state == 2)
		{
			vector<vector<CString>>res;
			mdb->excutecmd(_T("select * from friend;"), res);
			Friend::getfriend(res);
		}
	}
}

bool USocketServer::startsvr(short port)
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
		initdb();
		getdata();
		return bl;
	}
	else
	{
		AfxMessageBox(_T("创建套接字失败"));
		return false;
	}
}

void USocketServer::init()
{
	WSADATA wa;
	if (WSAStartup(MAKEWORD(2, 2), &wa) != 0)
	{
		AfxMessageBox(_T("sock初始化失败！"));
	}
}


void USocketServer::closesvr()
{
	ShutDown();
	Close();
	vector<USocketServer*>arr = clients;
	for (int i = 0;i < arr.size();i++)
	{

		arr[i]->OnClose(0);
	}

	if (mdb != NULL)
	{
		mdb->close();
		delete mdb;
		mdb = NULL;
	}
	
}

bool USocketServer::connectsvr()
{
	TCHAR sz[1024];
	CString file = getpath() + _T("svr.ini"); //配置读取文件
	bool bl = GetPrivateProfileStringW(_T("服务"), _T("ip"), _T("127.0.0.1"), sz, 1024, file);
	mip = sz;
	bl = GetPrivateProfileStringW(_T("服务"), _T("端口"), _T("10001"), sz, 1024, file);
	mport = _wtoi(sz);

	bl = Connect(mip, mport);
	return bl;
}

void USocketServer::OnSend(int nErrorCode)
{
	
}
void USocketServer::OnAccept(int nErrorCode)
{
	USocketServer *sock = new USocketServer();
	int sockAddrLen = sizeof(SOCKADDR);
	if (Accept(*(sock), (LPSOCKADDR)&(sock->IpAddr), &sockAddrLen)) //接收连接
	{
		sock->AsyncSelect(FD_READ | FD_WRITE | FD_CLOSE); //设置感兴趣事件
		addclient(sock);
		sock->psvr = this;
		sock->OnConnect(0);

		CString cs = getaddr(sock->IpAddr);
		CString str;
		str.Format(_T("%s连接 %d"),cs,clients.size());
		showinfo(str);
	}
	else
	{
		if (sock != NULL)
		{
			delete sock;
		}
	}
}
void USocketServer::OnConnect(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		state = 1;
		
		int nBuf = 1024 * 100;
		int nBufLen = sizeof(nBuf);
		int nRe = setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nBuf, nBufLen);

		//检查缓冲区是否设置成功
		nRe = getsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nBuf, &nBufLen);
		
		nRe = setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nBuf, nBufLen);

		//检查缓冲区是否设置成功
		nRe = getsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nBuf, &nBufLen);
		if (hrecv != NULL)
		{
			//TerminateThread(hrecv, 0);
			hrecv = NULL;
		}
		//hrecv=CreateThread(NULL, 0, threceive, this, 0, NULL);
	}
	else
	{
		state = 0;
		connectsvr();
	}
	
}
void USocketServer::OnClose(int nErrorCode)
{
	if (psvr != NULL)
	{
		psvr->OnClientOffline(this);
	}
	state = 0;
}
void USocketServer::OnClientOffline(USocketServer*sock)
{
	CString cs = getaddr(sock->IpAddr);
	CString str;
	del(sock);
	str.Format(_T("%s断开,%d"), cs, clients.size());
	showinfo(str);
	senddata();
}
void USocketServer::del(USocketServer*sock)
{
	for (vector<USocketServer*>::iterator it=clients.begin();it!=clients.end();it++)
	{
		if (*it == sock)
		{
			clients.erase(it);
			delete sock;
			break;
		}
	}
}

CString USocketServer::getpath()
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

void USocketServer::showinfo(CString cs)
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

void USocketServer::initdb()
{
	if (mdb == NULL)
	{
		mdb = new CMysql();
	}
	if (mdb != NULL)
	{
		mdb->login(true);
	}

	
}

int USocketServer::sendcmd(int cmd, CString cs)
{
	string str = UnicodeToAscii(cs.GetBuffer()).data();
	int r = sendcmd(cmd, str.c_str(), str.size());
	return r;
}

int USocketServer::sendcmd(int cmd, const char*pt, int len)
{
	if (m_hSocket != INVALID_SOCKET)
	{
		char *psnd = new char[len + 10];
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
		delete[]psnd;
		return r;
	}
	return -1;
}

CString USocketServer::getstr(vector<CString>&vec, CString dep /*= _T("\r\n")*/)
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

CString USocketServer::getstr(vector<vector<CString>>&vec, CString dep /*= _T("\r\n")*/)
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
CString USocketServer::getfriends(int id)
{
	CString cs = _T("");
	CString str;
	LockClient(true);
	for (int i = 0;i < Friend::friends.size();i++)
	{
		if (Friend::friends[i].uid1 == id || Friend::friends[i].uid2 == id)
		{
			str = Friend::friends[i].getstr();
			cs += str;
		}
	}
	LockClient(false);
	return cs;
}
CString USocketServer::getusers()
{
	CString cs = _T("");
	CString str;
	LockClient(true);
	for (int i = 0;i < clients.size();i++)
	{
		if (clients[i] != NULL)
		{
			str = clients[i]->muser.getuser();
			cs += str;
		}
	}
	LockClient(false);
	return cs;
}

void USocketServer::broadmsg(int cmd, CString cs)
{
	LockClient(true);
	for (int i = 0;i < clients.size();i++)
	{
		clients[i]->sendcmd(cmd, cs);
	}
	LockClient(false);
}

void USocketServer::LockClient(bool bl)
{
	if (bl)
	{
		EnterCriticalSection(&mcsclients);
	}
	else
	{
		LeaveCriticalSection(&mcsclients);
	}
}
void USocketServer::LockBuf(bool bl)
{
	return;
	if (bl)
	{
		EnterCriticalSection(&mcsbuf);
	}
	else
	{
		LeaveCriticalSection(&mcsbuf);
	}
}
int USocketServer::getint(char *sz)
{
	char arr[4] = { 0 };
	memcpy(arr, sz, 4);
	int *pn = (int *)arr;
	int n = *pn;
	n=ntohl(n);
	return n;
}

void USocketServer::readfile(CString fpath, CString &cs) //读文件
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

void USocketServer::readwords(CString cs, char c, vector<CString>&vec)
{
	vec.clear();
	cs = cs.Trim();
	cs = cs.TrimRight(c);
	if (cs.IsEmpty())
	{
		return;
	}
	cs += c;
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
void USocketServer::senddata()
{
	getdata();
	CString str = _T("USERS#");
	str += getusers();
	broadmsg(USERS, str);


	LockClient(true);
	for (int i = 0;i < clients.size();i++)
	{
		str = _T("FRIFENDS#");
		if (clients[i] == NULL)
		{
			continue;
		}
		str += getfriends(clients[i]->muser.uid);
		if (str.GetLength() > 9)
		{
			clients[i]->sendcmd(FRIFENDS, str);
		}
		
	}
	LockClient(false);
	
	
}
void  USocketServer::onfriend(vector<CString>vec)
{
	if (vec.size() >= 3)
	{
		CString str;
		CString res = _T("");
		USocketServer *c1 = findclient(vec[0]);
		USocketServer *c2 = findclient(vec[1]);
		if (c1 == NULL || c2 == NULL)
		{
			vec[2] = _T("对方不在线");
			str = getstr(vec);
			str = _T("FRIFEND#") + str;
			c2->sendcmd(FRIFEND, str);
			showinfo(str);
		}
		else
		{
			if (vec[2] == _T("add"))
			{
				vec[2] = _T("addyou");

				str = getstr(vec);
				str = _T("FRIFEND#") + str;
				c2->sendcmd(FRIFEND, str);
				showinfo(str);
			}
			else if (vec[2] == _T("yes"))
			{
				getdata();
				int id = Friend::find(c1->muser.uid, c2->muser.uid);
				if (id != -1)
				{
					vec[2] = _T("已经是好友");
				}
				else
				{
					res = mdb->addfriend(c1->muser.uid, c2->muser.uid);
					if (res == SUCC)
					{
						vec[2] = _T("添加成功");
					}
					else
					{
						vec[2] = _T("添加失败,") + res;
					}
				}
				str = getstr(vec);
				str = _T("FRIFEND#") + str;
				c2->sendcmd(FRIFEND, str);
				c1->sendcmd(FRIFEND, str);
				showinfo(str);
			}

			else if (vec[2] == _T("no"))
			{
				vec[2] = _T("已拒绝 ") + vec[0];
				str = getstr(vec);
				str = _T("FRIFEND#") + str;
				c2->sendcmd(FRIFEND, str);

				vec[2] = vec[1] + _T(" 拒绝你的好友申请");
				str = getstr(vec);
				str = _T("FRIFEND#") + str;
				c1->sendcmd(FRIFEND, str);
				showinfo(str);
			}
			if (vec[2] == _T("del"))
			{
				getdata();
				int id = Friend::find(c1->muser.uid, c2->muser.uid);

				if (id == -1)
				{
					vec[2] = _T("非好友删除失败");
				}
				else
				{
					res = mdb->delfriend(c1->muser.uid, c2->muser.uid);
					if (res == SUCC)
					{
						vec[2] = _T("删除成功");
						str = getstr(vec);
						str = _T("FRIFEND#") + str;
						c1->sendcmd(FRIFEND, str);

						vec[2] = _T("被删除成功");
						str = getstr(vec);
						str = _T("FRIFEND#") + str;
						c2->sendcmd(FRIFEND, str);
					}
					else
					{
						vec[2] = _T("删除失败,") + res;

						str = getstr(vec);
						str = _T("FRIFEND#") + str;
						c1->sendcmd(FRIFEND, str);
					}
				}


				showinfo(str);
			}
		}

	}

}

USocketServer * USocketServer::findclient(CString n)
{
	USocketServer *res = NULL;
	LockClient(true);
	for (int i = 0;i < clients.size();i++)
	{
		if (clients[i] != NULL)
		{
			if (clients[i]->muser.act == n)
			{
				res = clients[i];
				break;
			}
		}
	}
	LockClient(true);
	return res;
}

UFile* USocketServer::findfile(CString fn)
{
	for (int i = 0;i < files.size();i++)
	{
		if (fn == files[i]->name)
		{
			return files[i];
		}
	}
	return NULL;
}
bool USocketServer::clearfile(UFile*pfile)
{
	if (pfile == NULL)
	{
		for (int i = 0;i < files.size();i++)
		{
			if (files[i] != NULL)
			{
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
		for (vector<UFile*>::iterator it=files.begin();it!=files.end();it++)
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

void USocketServer::onchat_1n(CString cs)
{
	vector<CString>vec;
	User::readwords(cs, ',', vec, 3);
	
	if (vec.size() >= 3)
	{
		CString str = _T("CHAT2#") + cs;
		broadmsg(CHAT2, str);
	}
}
void USocketServer::onchat_11(CString cs)
{
	vector<CString>vec;
	User::readwords(cs, ',', vec, 3);
	if (vec.size() >= 3)
	{
		CString sstr = _T("CHAT#") + cs;
		USocketServer *c1 = findclient(vec[0]);
		USocketServer *c2 = findclient(vec[1]);
	
		if (c1 != NULL)
		{
			c1->sendcmd(CHAT, sstr);
		}
		if (c2!= NULL)
		{
			c2->sendcmd(CHAT, sstr);
		}
		showinfo(_T("CHAT") + cs);
	}
	
}
void USocketServer::process(int cmd, CString cs)
{
	if (psvr == NULL)
	{
		return;
	}
	vector< vector<CString>>data;
	vector<CString>vec;
	readwords(cs, ',', vec);
	CString res = _T("");
	CString str;
	User *u = NULL;
	USocketServer*ps = NULL;
	switch (cmd)
	{
	case  REG:
		if (psvr->mdb != NULL)
		{
			res = psvr->mdb->reg(vec);
			if (res == SUCC)
			{
				res = _T("注册#成功");
			}
			else
			{
				res = _T("注册#") + res;
			}
			sendcmd(cmd, res);
			showinfo(cs+res);
		}
		else
		{
			sendcmd(cmd, _T("注册#注册失败数据库错误!"));
			showinfo(cs+ _T("注册失败数据库错误!"));
		}
		
		break;
	case  LOGIN:

		if (psvr->mdb != NULL)
		{
			ps = psvr->findclient(vec[0]);
			if (ps != NULL)
			{
				sendcmd(cmd, _T("登录#重复登录"));
				showinfo(cs + _T("重复登录!"));
			}
			else
			{
				res = psvr->mdb->login(vec, data);
				if (res == SUCC)
				{
					str = getstr(data);
					str = _T("登录#成功#") + str;
					sendcmd(cmd, str);
					muser = User::getuser(data[0]);
					psvr->senddata();

					showinfo(cs + _T("登录成功!"));
				}
				else
				{
					res = _T("登录#") + res;
					sendcmd(cmd, res);
				}
			}
			
		}
		else
		{
			sendcmd(cmd, _T("登录#失败数据库错误!"));
		}

		break;
	case  CHAT:
		if (vec.size() >= 3)
		{
			showinfo(_T("CHAT") + cs);
			psvr->onchat_11(cs);
		}
		else
		{
			sendcmd(cmd, _T("单聊发送失败，数据错误!"));
		}
		break;
	case  CHAT2:
		if (vec.size() >= 2)
		{
			showinfo(_T("CHAT2") + cs);
			psvr->onchat_1n(cs);
		}
		else
		{
			sendcmd(cmd, _T("群聊发送失败，数据错误!"));
		}
		break;
	case  FRIFEND:
		if (vec.size() >= 3)
		{
			psvr->onfriend(vec);
		}
		else
		{
			sendcmd(FRIFEND, _T("ADDFRES#添加失败，数据错误!#err"));
		}
		break;
	default:
		break;
	}
}

void USocketServer::parsedata()
{

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
				if (cmd ==FILE)
				{
					memcpy(tmp, mbuf + ibeg, nsize + 10);  //数据
					OnFile(cmd, tmp, nsize);
				
				}
				else
				{
					
					memcpy(tmp, mbuf + ibeg + 10, nsize);  //数据
					msg = AsciiToUnicode(tmp).data();
					process(cmd, msg);	//消息处理
				}
				ibeg += 10;
				ibeg += nsize;        //数据开始位置更新
				datasize = iend - ibeg;
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
				iend = 0;
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
	
}
void  USocketServer::senddata(int cmd, char *data, int len)
{
	CString str;
	int r = 0;
	int s = 0;
	int e = 0;
	while (s < len)
	{
		r = Send(data, len);
		if (r > 0)
		{
			s += r;
			if (s != r)
			{
				str.Format(_T("##############%d/%d"), r, len);
				psvr->showinfo(str);
			}
		}
		if (r<0)
		{
			e=GetLastError();
			if (e == WSAEWOULDBLOCK)
			{
				continue;
			}
			else
			{
				psvr->showinfo(_T("err"));
				break;
			}
		}
		if (state == 0)
		{
			psvr->showinfo(_T("off"));
			break;
		}
		
	}

}
void USocketServer::OnFile(int cmd, char *data, int len)
{
	if (len < HEADLEN)
	{
		
	}
	else
	{
		CString str;
		CString res;
		vector<CString>vec;
		int cmd1 = data[DATAPOS];
		data[HEADLEN+DATAPOS-1] = 0;
		CString cs = AsciiToUnicode(data + DATAPOS +1).data();
		readwords(cs, ',', vec);
		if (vec.size() >= 4)
		{
			if (psvr != NULL)
			{
				USocketServer *tmp = NULL;
				if (muser.act == vec[0])
				{
					tmp = psvr->findclient(vec[1]);
					if (cmd1 == 2)
					{
						if (tmp != NULL)
						{
							num += (len - HEADLEN);
						
						}
					}
					if (cmd1 == 1)
					{
						num = 0;
						bs = false;
					}
					if (cmd1 == 10)
					{
						psvr->showinfo(muser.act + _T("cancel1"));
					}
					if (cmd1 == 4)
					{
						bs = true;
					}
					if (bs)
					{
						str.Format(_T("%s %d/%d"), vec[2],num, _wtoi(vec[3]));
						psvr->showinfo(str);
					}
				}
				else if (muser.act == vec[1])
				{
					tmp = psvr->findclient(vec[0]);
					if (cmd1 == 10)
					{
						psvr->showinfo(muser.act + _T("cancel2"));
					}
				}
				if (tmp != NULL)
				{
					tmp->senddata(cmd, data, len + 10);
				}
				else
				{
					if (num >= 0)
					{
						num = -1;
						if (muser.act == vec[0])
						{
							tmp = psvr->findclient(vec[0]);
							if (cmd1 == 1 )
							{
								data[DATAPOS] = 5;
								tmp->Send(data, len + 10);
							}
							if (cmd1 == 2)
							{

								data[DATAPOS] = 5;
								tmp->Send(data, len + 10);
							}
						}
						else
						{

						}
						
					}
					
				}
		
			}
			else
			{
				sendcmd(FILE, _T("FILE,发送失败,服务错误!,") + cs);
			}
		
		}

	}
}
DWORD WINAPI USocketServer::threceive(LPVOID p)
{
	USocketServer*psock = (USocketServer*)p;
	if (psock == NULL)
	{
		return 0;
	}
	while (psock->state!=0)
	{
		psock->parsedata();
		Sleep(10);

	}
	return 0;
}
string USocketServer::Utf8ToAscii(const string& str)
{
	return UnicodeToAscii(Utf8ToUnicode(str));
}
string USocketServer::AsciiToUtf8(const string& str)
{
	return UnicodeToUtf8(AsciiToUnicode(str));
}
wstring USocketServer::Utf8ToUnicode(const string& str)
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
string USocketServer::UnicodeToUtf8(const wstring& wstr)
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
wstring USocketServer::AsciiToUnicode(const string& str)
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
string USocketServer::UnicodeToAscii(const wstring& wstr)
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
// SocketClient 成员函数
