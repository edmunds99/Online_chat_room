// fileProDlg.cpp: 实现文件
//

#include "pch.h"
#include "chatc.h"
#include "fileProDlg.h"
#include "afxdialogex.h"
#include "chatcDlg.h"

// fileProDlg 对话框

IMPLEMENT_DYNAMIC(fileProDlg, CDialogEx)

fileProDlg::fileProDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
	, mstr(_T(""))
{
	mfile = NULL;
	brec = false;
}

fileProDlg::~fileProDlg()
{
}

void fileProDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, mpro);
	DDX_Text(pDX, IDC_EDIT1, mstr);
}


BEGIN_MESSAGE_MAP(fileProDlg, CDialogEx)
	ON_MESSAGE(UMSG, OnMsg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &fileProDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDCANCEL2, &fileProDlg::OnBnClickedCancel2)
END_MESSAGE_MAP()
LRESULT fileProDlg::OnMsg(WPARAM w, LPARAM l)
{
	int n = (int)w;
	update();
	return 0;
}

// fileProDlg 消息处理程序


BOOL fileProDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	mpro.SetRange32(0, 1000);
	mpro.SetPos(0);
	update();
	SetTimer(1, 100, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void fileProDlg::update()
{
	if (mfile == NULL)
	{
		return;
	}
	if (mfile->bsend == 1)
	{
		if (!IsWindowVisible())
		{
			ShowWindow(SW_SHOW);
		}
	}
	if (mfile->size != 0)
	{
		UpdateData();
		CString cs;
		float f = mfile->rsize *100.0/ mfile->size;
		float f1 = 0;
		ULONGLONG u = GetTickCount64() - mfile->st;
		if (u != 0)
		{
			f1 = mfile->rsize / (u);
		}
		mpro.SetPos(10* f);
		cs = _T("");
		if (brec)
		{

			mstr.Format(_T("接收 %s(%s)"),mfile->name, mfile->sender);
			SetWindowTextW(mstr);
			mstr.Format(_T("进度%.1f%% %.1fkb/s"),  f, f1);
			if (mfile->bsend == 4)
			{
				if (mfile->rsize == mfile->size)
				{
					cs.Format(_T("\r\n接收 %s 发送的 %s 成功 %d/%d"), mfile->sender, mfile->name,
						mfile->rsize, mfile->size);
				}
				else
				{
					cs.Format(_T("\r\n接收 %s 发送的 %s 失败 %d/%d"), mfile->sender, mfile->name,
						mfile->rsize, mfile->size);
				}
				
				GetDlgItem(IDCANCEL)->SetWindowTextW(_T("关闭"));
			}
			else if (mfile->bsend == 10)
			{
				cs.Format(_T("\r\n接收%s失败,%s已取消发送"), mfile->name, mfile->sender);
				GetDlgItem(IDCANCEL)->SetWindowTextW(_T("关闭"));
			}
		}
		else
		{
			
			mstr.Format(_T("发送 %s(%s)"), mfile->name, mfile->recver);
			SetWindowTextW(mstr);

			mstr.Format(_T("进度%.1f%% %.1fkb/s"),  f, f1);

			if (mfile->bsend == 0) 
			{
				cs.Format(_T("\r\n发送 %s %d/%d完成"), mfile->path, mfile->rsize, mfile->size);
				GetDlgItem(IDCANCEL)->SetWindowTextW(_T("关闭"));
			}
			else if (mfile->bsend == 5)
			{
				cs.Format(_T("\r\n发送%s失败,%s断线"), mfile->name, mfile->recver);
				GetDlgItem(IDCANCEL)->SetWindowTextW(_T("关闭"));
			}
			else if (mfile->bsend == 6)
			{
				cs.Format(_T("\r\n发送%s失败,%s数据错误"), mfile->name, mfile->recver);
				GetDlgItem(IDCANCEL)->SetWindowTextW(_T("关闭"));
			}
			else if (mfile->bsend == 10)
			{
				cs.Format(_T("\r\n发送%s失败,%s已取消接收"), mfile->name, mfile->recver);
				GetDlgItem(IDCANCEL)->SetWindowTextW(_T("关闭"));
			}
		}
		if (mfile->bsend != 1)
		{
			if (mfile->file.m_hFile != INVALID_HANDLE_VALUE)
			{
				mfile->file.Close();
			}
			KillTimer(1);
			if (brec)
			{
				GetDlgItem(IDCANCEL2)->ShowWindow(SW_SHOW);
			}
			
		}
		else
		{
			GetDlgItem(IDCANCEL2)->ShowWindow(SW_HIDE);
		}
		mstr += cs;
		GetDlgItem(IDC_EDIT1)->SetWindowTextW(mstr);
	}
	
}

void fileProDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		update();
	}

	CDialogEx::OnTimer(nIDEvent);
}


void fileProDlg::OnBnClickedCancel()
{
	if (mfile == NULL)
	{
		return;
	}
	KillTimer(1);
	if (mfile->bsend == 1)
	{
		char sz[1024] = { 0 };
		sz[0] = 0x0a;
		sz[1] = 0xee;
		sz[DATAPOS] = 10;

		int n = USocketClient::FILE;
		n = htonl(n);
		memcpy(sz + 2, &n, sizeof(n));

		int n1 = htonl(HEADLEN);
		memcpy(sz + 6, &n1, sizeof(n1));

		string str = USocketClient::UnicodeToAscii(mfile->getstr().GetBuffer());
		memcpy(sz + DATAPOS + 1, str.c_str(), str.size());

		mfile->bsend = 10;
		while (mfile->mth!=NULL)
		{
			Sleep(50);
		}
		mfile->pclient->Send(sz, HEADLEN + DATAPOS);
		
		
	}
	else
	{
		

	}
	CchatcDlg::Inst->client.clearfile(mfile);
	mfile = NULL;
}


void fileProDlg::OnBnClickedCancel2()
{
	if (mfile != NULL)
	{
		CString str;
		str.Format(_T("/select, %s"), mfile->path);
		ShellExecute(NULL, _T("open"), _T("explorer.exe"), str, NULL, SW_SHOWNORMAL);
	}
}
