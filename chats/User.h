#pragma once
#include <vector>
using namespace std;
#define  CHATFILE _T("聊天.txt")
class Friend
{
public:
	static vector<Friend>friends;
	int uid1;  //用户
	int uid2;  //好友
	CString strtime; //添加时间

	Friend()
	{
		uid1 = -1;
		uid2 = -1;
		strtime = _T("");
	}
	CString getstr()
	{
		CString cs;
		cs.Format(_T("%d,%d,%s,\r\n"), uid1, uid2, strtime);
		return cs;
	}
	static void clear();
	static Friend getfriend(vector<CString>&vec);  //读取用户
	static void getfriend(CString cs);  //读取用户
	static void getfriend(vector<vector<CString>>&vec);  //读取用户
	static int find(int id1, int id2);  //读取用户
	static Friend readfriend(vector<CString>&vec);  //读取用户
};
class User
{
public:
	static vector<User>users;
	int uid;					//客户id
	CString act;				//用户名
	CString pwd;				//密码
	CString code;				//姓名
	CString phone;				//手机
	vector<User>friends;
	int state;

	User()
	{
		uid = -1;
		act = _T("");
		pwd = _T("");
		code = _T("");
		act = _T("");
		state = 0;
	}
	CString getinfo()
	{
		CString cs;
		cs.Format(_T("%s"),act);
		return cs;
	}
	CString getuser()
	{
		CString cs;
		cs.Format(_T("%d,%s,%s,%s,%s,\r\n"), uid, act, pwd, code, phone);
		return cs;
	}
	static void clear();
	static User getuser(vector<CString>&vec);  //读取用户
	static void getuser(CString cs);  //读取用户
	static void getuser(vector<vector<CString>>&vec);  //读取用户
	static int find(CString cs);//读取用户;
	static int find(int id);//读取用户;
	static void readwords(CString cs, char c, vector<CString>&vec,int n=-1);
	int findfriend(CString n);
	void delfriend(CString n);
	void setfriends(vector<Friend>arr);

	
};

class CChat
{
public:
	static CString lpath;
	static vector<CChat>chats;
	CString uid1;
	CString uid2;
	CString txt;
	CString strtime;
	CChat()
	{
		uid1 = _T("");
		uid2 = _T("");
		txt = _T("");
		strtime = _T("");
	}
	static wstring AsciiToUnicode(const string& str)
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
	static string UnicodeToAscii(const wstring& wstr)
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
	static void readfile(CString fpath, CString &cs) //读文件
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
		cs =AsciiToUnicode(arr).data();
		file.Close();
		delete[]arr;
	}
	static CString getpath()
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
	CString gettxt(CString sstrn=_T(""))
	{
		CString cs;
		if (uid2!=_T(""))
		{
			if (uid1 == sstrn)
			{
				cs.Format(_T("[%s]%s -> %s:%s\r\n"), strtime, _T("你"), uid2, txt);
			}
			else if (uid2 == sstrn)
			{
				cs.Format(_T("[%s]%s -> %s:%s\r\n"), strtime, uid1, _T("你"), txt);
			}
			else
			{
				cs.Format(_T("[%s]%s -> %s:%s\r\n"), strtime, uid1, uid2, txt);
			}
			
		}
		else
		{
			cs.Format(_T("[%s]%s:%s\r\n"), strtime,uid1, txt);
			if (uid1 == sstrn)
			{
				cs.Format(_T("[%s]%s:%s\r\n"), strtime, _T("你"), txt);
			}
			else
			{
				cs.Format(_T("[%s]%s:%s\r\n"), strtime, uid1, txt);
			}
		}

		return cs;
	}
	CString getstr()
	{
		CString cs;
		cs.Format(_T("%s,%s,%s,%s\r\n"), strtime, uid1, uid2, txt);
		return cs;
	}
	static void addchat(CString cs)
	{
		
		vector<CString>line;
		User::readwords(cs, ',', line,4);
		if (line.size() >= 4)
		{
			CChat tmp;
			tmp.strtime = line[0];
			tmp.uid1 = line[1];
			tmp.uid2 = line[2];
			tmp.txt = line[3];
			chats.push_back(tmp);
		}

	}
	static void  readchat(int id);
	static void savechat(int id,int t=-1, CString strn= _T(""))
	{
		CString file = getpath();
		CString str;
		str.Format(_T("%d_%s"), id,CHATFILE);

		file += str;
		vector<CString>tmp;
		CFile cf;
		vector<CChat>res;
		if (cf.Open(file, CFile::modeCreate | CFile::modeWrite))
		{
			string cs;
			for (int i = 0;i < CChat::chats.size();i++)
			{
				if (t==1)
				{
					if (CChat::chats[i].uid2 == _T(""))
					{
						continue;
					}
				}
				else if (t == 2)
				{
					if (CChat::chats[i].uid2 == strn|| CChat::chats[i].uid1==strn)
					{
						continue;
					}
				}
				cs =UnicodeToAscii(CChat::chats[i].getstr().GetBuffer()).data();
				cf.Write(cs.c_str(), cs.size());
				res.push_back(CChat::chats[i]);
			}
			cf.Close();
		}
		CChat::chats = res;
	}
	
};
#define  HEADLEN 200
#define  DATAPOS 10
class UFile
{
public:
	CString name;
	CString path;
	CString sender;
	CString recver;
	CFile file;
	int size;
	CAsyncSocket*pclient;
	int bsend;
	int rsize;
	CDialog *pDlg;
	ULONGLONG st;
	HANDLE mth;
	UFile()
	{
	
		mth = NULL;
		pDlg = NULL;
		bsend = false;
		rsize = 0;
		pclient = NULL;
		name = path = sender = recver = _T("");
	}
	int  init(bool br)
	{
		CString cs = getstr();
		if (br)
		{
			if (file.Open(path, CFile::modeCreate | CFile::modeWrite|CFile::shareDenyWrite))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if (file.Open(path,  CFile::modeRead|CFile::shareDenyWrite))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	CString getstr()
	{
		CString cs;
		cs.Format(_T("%s,%s,%s,%d"), sender, recver,
			name, size);
		return cs;
	}
	void clear()
	{
		pclient = NULL;
		if (file.m_hFile != INVALID_HANDLE_VALUE)
		{
			file.Close();
		}
		if (pDlg != NULL)
		{
			if (pDlg->m_hWnd != NULL)
			{
				pDlg->CloseWindow();
			}
			delete pDlg;
		}
		
	}
};