
// chatcDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "chatc.h"
#include "chatcDlg.h"
#include "afxdialogex.h"
#include "loginDlg.h"
#include "fileProDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "User.h"
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CchatcDlg 对话框


CchatcDlg *CchatcDlg::Inst = NULL;
CchatcDlg::CchatcDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATC_DIALOG, pParent)
	, strtxt(_T(""))
	, ncharttype(0)
{
	Inst = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	sel = -1;


}
void CchatcDlg::Onmsg(CString cs)
{
	if (Inst != NULL)
	{
		vector<CString>vec;
		vector<CString>vec1;
		USocketClient::readwords(cs, '#', vec);
		int len = vec.size();
		if (len > 0)
		{
			if (vec[0] == _T("网络"))
			{
				if (len > 1)
				{
					if (vec[1] == _T("断线"))
					{
						if (loginDlg::Inst == NULL)
						{
							Inst->login();
						}
					}

				}
			}
			else if (vec[0] == _T("USERS"))
			{
				if (len > 1)
				{
					User::getuser(vec[1]);
					Inst->showuser();
				}
				
			}
			else if (vec[0] == _T("FRIFENDS"))
			{
				if (len > 1)
				{
					Friend::getfriend(vec[1]);
					if (Inst->muser.uid != -1)
					{
						Inst->muser.setfriends(Friend::friends);
					}
					Inst->showuser();
				}
				
			}
			else if (vec[0] == _T("CHAT"))
			{
				if (len > 1)
				{
					Inst->addchat_11(vec[1]);
				}
				
			}
			else if (vec[0] == _T("CHAT2"))
			{
				if (len > 1)
				{
					Inst->addchat_1n(vec[1]);
				}

			}
			else if (vec[0] == _T("FRIFEND"))
			{
				if (len > 1)
				{
					Inst->onfriend(vec[1]);
				}
			}
		
		}
		
	}
}
void CchatcDlg::OnFile(vector<CString>vec)
{
	
}
void CchatcDlg::onfriend(CString cs)
{

	CString str;
	vector<CString>vec;
	USocketClient::readwords(cs, ',', vec);
	if (vec.size() >= 3)
	{
		if (vec[2] == _T("addyou"))
		{
			str.Format(_T("%s请求添加你为好友，同意？"),vec[0]);
			if (AfxMessageBox(str, MB_YESNO) == IDYES)
			{
				vec[2] = _T("yes");
			}
			else
			{
				vec[2] = _T("no");
			}
			str = USocketClient::getstr(vec);
			client.sendcmd(USocketClient::FRIFEND, str);
		}
		else if (vec[2] == _T("添加成功"))
		{
			if (muser.act == vec[0])
			{
				int id = User::find(vec[1]);
				if (id != -1)
				{
					muser.friends.push_back(User::users[id]);
				}
			}
			else if (muser.act == vec[1])
			{
				int id = User::find(vec[0]);
				if (id != -1)
				{
					muser.friends.push_back(User::users[id]);
				}
			}
			AfxMessageBox(vec[2]);
			showuser();
			showbtn();
		}
		else if (vec[2] == _T("删除成功"))
		{
			if (muser.act == vec[0])
			{
				muser.delfriend(vec[1]);
			}
			AfxMessageBox(vec[2]);
			showuser();
			showbtn();
		}
		else if (vec[2] == _T("被删除成功"))
		{
			if (muser.act == vec[1])
			{
				muser.delfriend(vec[0]);
			}
			showuser();
			showbtn();
		}
		else
		{
			AfxMessageBox(vec[2]);
		}
	}
}
void CchatcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, mlist);
	DDX_Control(pDX, IDC_EDIT1, editprichat);
	DDX_Text(pDX, IDC_EDIT2, strtxt);
	DDX_Radio(pDX, IDC_RADIO1, ncharttype);
	DDX_Control(pDX, IDC_EDIT5, editpubchat);
}

BEGIN_MESSAGE_MAP(CchatcDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CchatcDlg::OnDblclkList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CchatcDlg::OnClickList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CchatcDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CchatcDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON5, &CchatcDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &CchatcDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON6, &CchatcDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CchatcDlg::OnBnClickedButton7)
	ON_MESSAGE(UMSG, OnMsg)
END_MESSAGE_MAP()

LRESULT CchatcDlg::OnMsg(WPARAM w, LPARAM l)
{
	UFile *tmp = (UFile*)w;
	if (tmp != NULL)
	{
		if (tmp->pDlg->m_hWnd == NULL)
		{
			tmp->pDlg->Create(IDD_DIALOG3);
			tmp->pDlg->CenterWindow();
		}
		Sleep(10);
		tmp->pDlg->ShowWindow(SW_SHOW);
	}
	return 1;
}
// CchatcDlg 消息处理程序

BOOL CchatcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	DWORD dw = mlist.GetExtendedStyle();
	dw |= LVS_EX_GRIDLINES;
	dw |= LVS_EX_FULLROWSELECT;
	mlist.SetExtendedStyle(dw);
	mlist.InsertColumn(0, _T("在线用户"),LVCFMT_CENTER, 150);
	AfxSocketInit();
	
	client.Create();
	client.connectsvr();
	login();
	GetDlgItem(IDC_BUTTON4)->EnableWindow(false);

	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CchatcDlg::login()
{
	ShowWindow(SW_HIDE);
	mlist.DeleteAllItems();
	SetWindowTextW(_T(""));

	loginDlg dlg;
	dlg.DoModal();
	
	if (dlg.res == IDOK)
	{
		CenterWindow();
		SetWindowTextW(muser.act);
		ShowWindow(SW_SHOW);
		client.muser = muser;
		CChat::readchat(muser.uid);
		showchat(_T(""));
	}
	else
	{
		ShowWindow(SW_MINIMIZE);
		PostQuitMessage(0);
	}


}

void CchatcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CchatcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CchatcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CchatcDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

void  CchatcDlg::showuser(int t, CString str)
{
	editprichat.SetWindowTextW(_T(""));
	GetDlgItem(IDC_STATIC2)->SetWindowTextW(_T(""));
	bool bl = false;
	CString cs;
	mlist.DeleteAllItems();
	int j = 0;

	User *u;
	for (int i=0;i<User::users.size();i++)
	{
		cs = User::users[i].getinfo();
		if (muser.act == User::users[i].act)
		{
			continue;
		}
		int id = muser.findfriend(User::users[i].act);
		if (id != -1)
		{
			cs += _T("(好友)");
		}

		mlist.InsertItem(j,cs);
		mlist.SetItemData(j,i);
		if (sel == j)   //当前私聊
		{
			GetDlgItem(IDC_STATIC2)->SetWindowTextW(User::users[i].act);
			showchat(User::users[i].act);
		}
		j++;
		
	}
}
void CchatcDlg::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (sel >= 0 && sel < mlist.GetItemCount())
	{
		int n = mlist.GetItemData(sel);
		if (n >= 0 && n < User::users.size())
		{
			AfxMessageBox(User::users[n].act);
		}
		ncharttype = 1;
	}
	
	*pResult = 0;
}


void CchatcDlg::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	NMLISTVIEW*pt = (NMLISTVIEW*)pNMHDR;
	if (pt->iItem != -1)
	{
		sel = pt->iItem;
		int n = mlist.GetItemData(sel);
		if (n>=0&&n<User::users.size())
		{
			showchat(User::users[n].act);
			GetDlgItem(IDC_BUTTON4)->EnableWindow(true);
			GetDlgItem(IDC_STATIC2)->SetWindowTextW(User::users[n].act);
			int id= muser.findfriend(User::users[n].act);
			if (id == -1)
			{
				GetDlgItem(IDC_BUTTON4)->SetWindowTextW(_T("添加好友"));
			}
			else
			{
				GetDlgItem(IDC_BUTTON4)->SetWindowTextW(_T("删除好友"));
			}
		}
		
	}
	*pResult = 0;
}
void  CchatcDlg::showchat(CString act)
{

	CString txt;
	CString txt2;
	

	editprichat.SetWindowTextW(_T(""));
	CString res1=_T("");
	CString res2=_T("");
	for (int i=0;i<CChat::chats.size();i++)
	{

		if (CChat::chats[i].uid2==_T(""))
		{
			res1 += CChat::chats[i].gettxt(muser.act);
		}
		else
		{
			if ((CChat::chats[i].uid1 == act || CChat::chats[i].uid2 == act)
				&& (CChat::chats[i].uid1 == muser.act || CChat::chats[i].uid2 == muser.act))
			{

				res2 += CChat::chats[i].gettxt(muser.act);
			}
		
		}
	}
	editpubchat.SetWindowTextW(res1);
	editpubchat.LineScroll(editpubchat.GetLineCount() - 1);
	txt2 += res2;
	editprichat.SetWindowTextW(txt2);
	editprichat.LineScroll(editprichat.GetLineCount() - 1);
}



void  CchatcDlg::addchat_1n(CString cs)
{
	COleDateTime t = COleDateTime::GetCurrentTime();
	CString strt = t.Format(_T("%Y-%m-%d %H:%M:%S"));
	CChat::addchat(strt + _T(",") + cs);  //添加保存聊天
	CChat::savechat(muser.uid);
	bool bshow = false;
	if (sel >= 0 && sel < mlist.GetItemCount())
	{
		int id = mlist.GetItemData(sel);
		if (id >= 0 && id < User::users.size())
		{
			showchat(User::users[id].act); //当前私聊
			bshow = true;
		}
	}
	if (!bshow)
	{
		showchat(); //当前私聊
	}
	
}
void CchatcDlg::addchat_11(CString cs)
{
	COleDateTime t = COleDateTime::GetCurrentTime();
	CString strt = t.Format(_T("%Y-%m-%d %H:%M:%S"));
	CChat::addchat(strt + _T(",") + cs); //添加保存聊天
	CChat::savechat(muser.uid); 

	if (sel >= 0 && sel < mlist.GetItemCount())
	{
		int id = mlist.GetItemData(sel);
		if (id >= 0 && id < User::users.size())
		{
			showchat(User::users[id].act); //当前私聊
		}
	}
	
	
}

void CchatcDlg::OnBnClickedButton1()
{
	UpdateData();
	strtxt.Trim();
	if (strtxt.IsEmpty())
	{
		return;
	}
	CString cs;
	if (ncharttype == 0)
	{
		cs.Format(_T("%s,,%s"), muser.act, strtxt);
		client.sendcmd(USocketClient::CHAT2, cs);
		strtxt = _T("");
		UpdateData(FALSE);
	}
	else
	{
		if (sel >= 0 && sel < mlist.GetItemCount())
		{
			int id = mlist.GetItemData(sel);
			if (id >= 0 && id < User::users.size())
			{
				if (!strtxt.IsEmpty())
				{
					int i = muser.findfriend(User::users[id].act);
					if (i == -1)
					{
						AfxMessageBox(_T("请先添加 ")+ User::users[id].act+_T(" 为好友"));
					}
					else
					{
						cs.Format(_T("%s,%s,%s"), muser.act, User::users[id].act, strtxt);
						client.sendcmd(USocketClient::CHAT, cs);
						strtxt = _T("");
						UpdateData(FALSE);
					}
				
					//addchat(cs);
				}
			}
			else
			{
				AfxMessageBox(_T("数据错误"));

			}

		}
		else
		{
			AfxMessageBox(_T("选择用户"));
		}
	}
	
	
}


void CchatcDlg::OnBnClickedButton3()
{
	client.Close();
	CchatcDlg::OnOK();
}


void CchatcDlg::OnBnClickedButton5()
{
	UpdateData();
	CChat::savechat(muser.uid, 1);
	editpubchat.SetWindowTextW(_T(""));
}
void CchatcDlg::showbtn()
{
	if (sel >= 0 && sel < mlist.GetItemCount())
	{
		int id = mlist.GetItemData(sel);
		if (id >= 0 && id < User::users.size())
		{
			showchat(User::users[id].act); //当前私聊
			GetDlgItem(IDC_STATIC2)->SetWindowTextW(User::users[id].act);

			int n = muser.findfriend(User::users[id].act);
			if (n != -1)
			{
				GetDlgItem(IDC_BUTTON4)->SetWindowTextW(_T("删除好友"));
			}
			else
			{
				GetDlgItem(IDC_BUTTON4)->SetWindowTextW(_T("添加好友"));
			}
		}
	}
}

void CchatcDlg::OnBnClickedButton4()
{
	UpdateData();
	CString cs;
	if (sel >= 0 && sel < mlist.GetItemCount())
	{
		int id = mlist.GetItemData(sel);
		if (id>=0&&id<User::users.size())
		{
			
			int n = muser.findfriend(User::users[id].act);
			if (n == -1)
			{
				GetDlgItem(IDC_BUTTON4)->SetWindowTextW(_T("添加好友"));
				cs.Format(_T("%s,%s,add"), muser.act, User::users[id].act);
				client.sendcmd(USocketClient::FRIFEND, cs);
			}
			else
			{
				GetDlgItem(IDC_BUTTON4)->SetWindowTextW(_T("删除好友"));
				cs.Format(_T("%s,%s,del"), muser.act, User::users[id].act);
				client.sendcmd(USocketClient::FRIFEND, cs);
			}
		}
		else
		{
			AfxMessageBox(_T("数据错误"));

		}

	}
	else
	{
		AfxMessageBox(_T("选择用户"));
	}
}


void CchatcDlg::OnBnClickedButton6()
{
	editprichat.SetWindowTextW(_T(""));
	if (sel >= 0 && sel < mlist.GetItemCount())
	{
		int id = mlist.GetItemData(sel);
		if (id != -1)
		{
			CChat::savechat(muser.uid, 2, User::users[id].act);
		}
		

	}

}


void CchatcDlg::OnBnClickedButton7()
{
;
	UpdateData();
	if (ncharttype == 0)
	{
		AfxMessageBox(_T("选择私聊用户发送文件"));
		return;
	}
	CString cs;
	if (sel >= 0 && sel < mlist.GetItemCount())
	{
		int id = mlist.GetItemData(sel);
		if (id >= 0 && id < User::users.size())
		{
			int i = muser.findfriend(User::users[id].act);
			if (i == -1)
			{
				AfxMessageBox(_T("请先添加 ") + User::users[id].act + _T(" 为好友"));
			}
			else
			{
				CFileDialog dlg(TRUE);
				if (dlg.DoModal() == IDOK)
				{

					int n = 0;
					CFileStatus s;

					if (CFile::GetStatus(dlg.GetPathName(),s))
					{
						n = s.m_size;
					}
					cs.Format(_T("%s,%s,%s,%d"), muser.act, User::users[id].act,
						dlg.GetPathName(), n);
					client.SendFile(cs);


					cs.Format(_T("%s,%s,文件:%s"), muser.act, User::users[id].act,
						dlg.GetPathName(), 0);
					client.sendcmd(USocketClient::CHAT, cs);
				}
			}
		}
		else
		{
			AfxMessageBox(_T("数据错误"));

		}

	}
	else
	{
		AfxMessageBox(_T("选择私聊用户"));
	}


	
}
