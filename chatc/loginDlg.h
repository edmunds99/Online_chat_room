#pragma once


// loginDlg 对话框

class loginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(loginDlg)

public:
	loginDlg(CWnd* pParent = nullptr);   // 标准构造函数
	static loginDlg *Inst;
	static void Onmsg(CString cs);
	virtual ~loginDlg();
	void setbtn(bool bl);
	int res;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString name;
	CString pwd;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDestroy();
	CStatic info;
	BOOL bpwd;
	afx_msg void OnBnClickedCheck1();
};
