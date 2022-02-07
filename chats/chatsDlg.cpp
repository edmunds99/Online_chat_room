
// chatsDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "chats.h"
#include "chatsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CchatsDlg 对话框



CchatsDlg::CchatsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATS_DIALOG, pParent)
	, port(_T("8888"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CchatsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, info);
	DDX_Text(pDX, IDC_EDIT2, port);
}

BEGIN_MESSAGE_MAP(CchatsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CchatsDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CchatsDlg::OnBnClickedButton2)
	ON_WM_DESTROY()
	ON_MESSAGE(UMSG, &CchatsDlg::onmessage)
	ON_BN_CLICKED(IDC_BUTTON3, &CchatsDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CchatsDlg 消息处理程序

BOOL CchatsDlg::OnInitDialog()
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

	//USocket::init();
	AfxSocketInit();
	svr.hwnd = m_hWnd;

	showdb();

	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CchatsDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CchatsDlg::OnPaint()
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
HCURSOR CchatsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CchatsDlg::OnBnClickedButton1()
{
	UpdateData();
	short s = _wtoi(port);
	if (svr.m_hSocket == INVALID_SOCKET)
	{
		bool bl=svr.startsvr(s);
		if (bl)
		{
			GetDlgItem(IDC_BUTTON1)->SetWindowTextW(_T("关闭服务"));
			svr.showinfo(_T("启动服务"));
		}
		else
		{
			GetDlgItem(IDC_BUTTON1)->SetWindowTextW(_T("启动服务"));
		}

	}
	else
	{
		svr.showinfo(_T("关闭服务"));
		svr.closesvr();
		GetDlgItem(IDC_BUTTON1)->SetWindowTextW(_T("启动服务"));
		GetDlgItem(IDC_BUTTON3)->SetWindowTextW(_T("登录数据库"));
	}
	showdb();
}

LRESULT CchatsDlg::onmessage(WPARAM w, LPARAM l)
{
	CString cs;
	info.GetWindowTextW(cs);
	for (int i = 0; i < svr.messages.size();i++)
	{
		cs += svr.messages[i];
		cs += _T("\r\n");
	}
	if (cs.GetLength() > 4096000)
	{
		cs.Empty();
	}

	svr.messages.clear();
	info.SetWindowTextW(cs);
	info.LineScroll(info.GetLineCount() - 1);
	return 0;
}
void CchatsDlg::OnBnClickedButton2()
{
	CDialogEx::OnCancel();
}


void CchatsDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	svr.closesvr();
	// TODO: 在此处添加消息处理程序代码
}
void CchatsDlg::showdb()
{
	if (svr.mdb == NULL)
	{
		GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
		return;
	}
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	if (svr.mdb->state == 0)
	{
		GetDlgItem(IDC_BUTTON3)->SetWindowTextW(_T("登录数据库"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON3)->SetWindowTextW(_T("退出数据库"));
	}
}

void CchatsDlg::OnBnClickedButton3()
{
	if (svr.mdb == NULL)
	{
		return;
	}
	if (svr.mdb->state==0)
	{
		CString err=svr.mdb->login(true);
		if (err != SUCC)
		 {
		  AfxMessageBox(err);
		 }
	}
	else
	{
		svr.mdb->close();
	}
	showdb();
}
