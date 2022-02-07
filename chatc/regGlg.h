#pragma once


// regGlg 对话框

class regGlg : public CDialogEx
{
	DECLARE_DYNAMIC(regGlg)

public:
	regGlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~regGlg();
	static regGlg*Inst;
	static void Onmsg(CString cs);
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString name;
	CString pwd;
	CString code;
	CString phone;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnDestroy();
};
