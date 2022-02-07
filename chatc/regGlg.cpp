// regGlg.cpp: 实现文件
//

#include "pch.h"
#include "chatc.h"
#include "regGlg.h"
#include "afxdialogex.h"
#include "chatcDlg.h"

// regGlg 对话框

IMPLEMENT_DYNAMIC(regGlg, CDialogEx)
regGlg*regGlg::Inst = NULL;;
regGlg::regGlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	, name(_T(""))
	, pwd(_T(""))
	, code(_T(""))
	, phone(_T(""))
{
	Inst = this;
}
void regGlg::Onmsg(CString cs)
{
	if (Inst != NULL)
	{
		vector<CString>vec;
		USocketClient::readwords(cs, '#', vec);
		int len = vec.size();
		if (len > 0)
		{
			Inst->GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
			if (vec[0] == _T("网络"))
			{
				if (len > 1)
				{
					if (vec[1] == _T("断线"))
					{
						Inst->PostMessageW(WM_CLOSE);
					}
					
				}
				
			}
			else  if (vec[0] == _T("注册"))
			{
				if (len > 1)
				{
					AfxMessageBox(vec[1]);
				}
				
			}
			
		}
	}
}
regGlg::~regGlg()
{
}

void regGlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, name);
	DDX_Text(pDX, IDC_EDIT2, pwd);
	DDX_Text(pDX, IDC_EDIT3, code);
	DDX_Text(pDX, IDC_EDIT4, phone);
}


BEGIN_MESSAGE_MAP(regGlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &regGlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &regGlg::OnBnClickedButton2)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// regGlg 消息处理程序


BOOL regGlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void regGlg::OnBnClickedButton3()
{
	CDialogEx::OnCancel();
}


void regGlg::OnBnClickedButton2()
{
	UpdateData();
	name.Trim();
	pwd.Trim();
	code.Trim();
	phone.Trim();
	if (name.IsEmpty() || pwd.IsEmpty())
	{
		AfxMessageBox(_T("请输入用户名和密码"));
		return;
	}
	int n = name.FindOneOf(_T("~!#$%^&*()_+<>|\,."));
	if (n!=-1)
	{
		AfxMessageBox(_T("用户名存在特殊字符"));
		return;
	}
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	CString cs;
	cs.Format(_T("%s,%s,%s,%s,"),name,pwd,code,phone);
	CchatcDlg::Inst->client.sendcmd(USocketClient::REG,cs);
}


void regGlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	Inst = NULL;
	// TODO: 在此处添加消息处理程序代码
}
