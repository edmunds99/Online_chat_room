// loginDlg.cpp: 实现文件
//

#include "pch.h"
#include "chatc.h"
#include "loginDlg.h"
#include "afxdialogex.h"
#include "regGlg.h"
#include "chatcDlg.h"
// loginDlg 对话框

IMPLEMENT_DYNAMIC(loginDlg, CDialogEx)
loginDlg *loginDlg::Inst=NULL;
loginDlg::loginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, name(_T(""))
	, pwd(_T(""))
	, bpwd(false)
{
	Inst = this;
	res = IDCANCEL;
}

void loginDlg::Onmsg(CString cs)
{
	if (Inst != NULL)
	{
		vector<CString>vec;
		USocketClient::readwords(cs, '#', vec);
		int len = vec.size();
		if (len > 0)
		{
			Inst->setbtn(true);
			if (vec[0] == _T("网络"))
			{
				if (len > 1)
				{
					if (vec[1] == _T("连接成功"))
					{
						Inst->info.SetWindowTextW(_T("连接服务成功"));
					}
					else if (vec[1] == _T("断线"))
					{
						Inst->setbtn(false);
					}
				}
			}
			else if (vec[0] == _T("登录"))
			{
				if (len > 1)
				{
					if (vec[1] == _T("成功"))
					{
						if (len > 2)
						{
							vector<CString>vec1;
							USocketClient::readwords(vec[2], ',', vec1);
							CchatcDlg::Inst->muser = User::getuser(vec1);
							Inst->res = IDOK;
							Inst->PostMessageW(WM_CLOSE, IDOK, IDOK);
						}
						
					}
					else
					{
						AfxMessageBox(vec[1]);
					}
					
				}
				
			}
	
		}
		
	}
}
void loginDlg::setbtn(bool bl)
{
	GetDlgItem(IDC_BUTTON1)->EnableWindow(bl);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(bl);
}
loginDlg::~loginDlg()
{
}

void loginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, name);
	DDX_Text(pDX, IDC_EDIT2, pwd);
	DDX_Control(pDX, IDC_STATIC1, info);
	DDX_Check(pDX, IDC_CHECK1, bpwd);
}


BEGIN_MESSAGE_MAP(loginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &loginDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &loginDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &loginDlg::OnBnClickedButton1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK1, &loginDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// loginDlg 消息处理程序


BOOL loginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	OnBnClickedCheck1();
	if(CchatcDlg::Inst->client.state==1)
	{
		setbtn(true);
	}
	else
	{
		setbtn(false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void loginDlg::OnBnClickedButton1()
{
	UpdateData();
	name.Trim();
	pwd.Trim();
	if (name.IsEmpty() || pwd.IsEmpty())
	{
		AfxMessageBox(_T("请输入用户名和密码"));
		return;
	}
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	CString cs;
	cs.Format(_T("%s,%s,"), name, pwd);
	CchatcDlg::Inst->client.sendcmd(USocketClient::LOGIN, cs);
}


void loginDlg::OnBnClickedButton3()
{
	CDialogEx::OnCancel();
}


void loginDlg::OnBnClickedButton2()
{
	regGlg dlg;
	dlg.DoModal();
}




void loginDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	Inst = NULL;
	// TODO: 在此处添加消息处理程序代码
}


void loginDlg::OnBnClickedCheck1()
{
	UpdateData();

	CEdit *pedit = (CEdit *)GetDlgItem(IDC_EDIT2);
	if (!bpwd)
	{
		pedit->SetPasswordChar('*');
	}
	else
	{
		pedit->SetPasswordChar(0);
	}
	pedit->SetFocus();
}
