#include "pch.h"
#include "CMysql.h"

#ifdef _DEBUG
#define New   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

bool CMysql::binit=false;
#pragma comment(lib,".\\lib\\libmysql.lib")
#pragma comment(lib,".\\lib\\mysqlclient.lib")
#pragma comment(lib,".\\lib\\mysqlservices.lib")
CString CMysql::mdir=_T("");
string CMysql::UnicodeToAscii(const wstring& wstr)
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

CMysql::CMysql()
{
	mck = false;
	pcon = NULL;
	mhost = _T("");
	muser = _T("");
	mpwd = _T("");
	mdb = _T("");
	port = 0;
	init();
}

CMysql::CMysql(CString cs)
{
	charset = cs;
	pcon = NULL;
	mhost = _T("");
	muser = _T("");
	mpwd = _T("");
	mdb = _T("");
	port = 0;
	init();
}

string CMysql::Utf8ToAscii(const string& str) {
	return UnicodeToAscii(Utf8ToUnicode(str));
}
string CMysql::AsciiToUtf8(const string& str)
{
	return UnicodeToUtf8(AsciiToUnicode(str));
}

wstring CMysql::AsciiToUnicode(const string& str)
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
string CMysql::UnicodeToUtf8(const wstring& wstr)
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
wstring CMysql::Utf8ToUnicode(const string& str)
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
void CMysql::readini()
{
	TCHAR sz[1024];
	CString file = mdir + _T("db.ini");
	bool bl=GetPrivateProfileStringW(_T("db"), _T("主机"), _T("127.0.0.1"), sz, 1024, file);
	mhost = sz;
	bl = GetPrivateProfileStringW(_T("db"), _T("端口"), _T("3306"), sz, 1024, file);
	port = _wtoi(sz);
	bl = GetPrivateProfileStringW(_T("db"), _T("用户名"), _T("root"), sz, 1024, file);
	muser = sz;
	bl = GetPrivateProfileStringW(_T("db"), _T("密码"), _T("123"), sz, 1024, file);
	mpwd = sz;
	bl = GetPrivateProfileStringW(_T("db"), _T("数据库"), _T("studyroom"), sz, 1024, file);
	mdb = sz;
}
void CMysql::saveini(CString h,int p,CString u,CString w,CString d)
{
	CString file = mdir + _T("db.ini");
	WritePrivateProfileStringW(_T("db"), _T("主机"), h,file);
	CString cs;
	cs.Format(_T("%d"),p);
	WritePrivateProfileStringW(_T("db"), _T("端口"), cs, file);
	WritePrivateProfileStringW(_T("db"), _T("用户名"), u, file);
	WritePrivateProfileStringW(_T("db"), _T("密码"), w, file);
	WritePrivateProfileStringW(_T("db"), _T("数据库"), d, file);
}
BOOL CMysql::init()
{
	state = 0;
	if (mdir.IsEmpty())//程序目录
	{
		AfxGetModuleFileName(AfxGetInstanceHandle(), mdir);
		int n = mdir.ReverseFind('\\');
		if (n != -1)
		{
			mdir = mdir.Left(n + 1);
		}
	}
	readini(); //数据库配置
	if (!binit)
	{
		if (mysql_library_init(0, NULL, NULL))  //初始化库
		{
			return false;
		}
		binit = true;
	}
	if (pcon == NULL)  //初始化连接
	{ 
		pcon = mysql_init(NULL);
	}
	if (pcon == NULL)
	{
		return false;
	}
	else
	{
		CString cs;
		if (!charset.IsEmpty())  //字符集
		{ 
			cs = setcharset(_T("utf8"));
		}
		return true;
	}	
}
CString CMysql::setcharset(CString cs)
{
	CString res = SUCC;
	if (pcon == NULL)
	{
		return _T("null");
	}
	string cset = UnicodeToAscii(cs.GetBuffer()).data();
	int r=mysql_options(pcon, MYSQL_SET_CHARSET_NAME, cset.c_str());
	if (r != 0)  //错误
	{
		res = geterr(); //错误信息
	}
	else
	{
		charset = cs;
	}
	return res;
}
void CMysql::close(bool bl)
{
	
	if (pcon != NULL)
	{
		mysql_close(pcon);
		pcon = NULL;
	}
	if (bl)   
	{
		mysql_library_end();
		binit = false;
	}
	state = 0;
	
}
bool CMysql::bconnect()  //是否连接数据库
{
	if (pcon == NULL)
	{
		return false;
	}
	if (mysql_ping(pcon) == 0)
	{
		return true;
	}
	return false;
}
CString CMysql::geterr() //查询错误信息
{
	CString err = SUCC;
	if (pcon == NULL)
	{
		err = _T("null");
		return err;
	}
	string cs = mysql_error(pcon);
	err = Utf8ToUnicode(cs).data();
	
	return err;
}
CString CMysql::excutecmd(CString cs, vector<vector<CString>> &records)
{
	records.clear();
	CString err = SUCC;
	if (pcon == NULL)
	{
		err = _T("null");
		return err;
	}
	string str = UnicodeToUtf8(cs.GetBuffer()).data();
	if (mysql_query(pcon, str.c_str()) != 0)
	{
		err = geterr();
	}
	else
	{
		
		MYSQL_RES *res = mysql_store_result(pcon);  //查询结果

		MYSQL_ROW row;
		if (res != NULL)
		{
			CString col;
			string cs;
			while ((row = mysql_fetch_row(res)) != NULL)	//每条记录
			{
				vector<CString> record;
				int len = mysql_num_fields(res);
				for (int i = 0; i < len;i++)   //记录列
				{
					cs = row[i];
					col = Utf8ToUnicode(cs).data();
					record.push_back(col);
				}
				records.push_back(record);   //保存记录

			}
			mysql_free_result(res);
		}
		else
		{
			err = geterr();
			if (err.IsEmpty())
			{
				err = SUCC;
			}
		}
	}
	return err;
}

CString CMysql::excutecmd(CString sql)
{

	CString err = SUCC;
	if (pcon == NULL)
	{
		err = _T("null");
		return err;
	}
	string str = UnicodeToUtf8(sql.GetBuffer()).data();
	if (mysql_query(pcon, str.c_str()) != 0)
	{
		err = geterr();
	}
	return err;
}


CString CMysql::login(bool bl)  
{
	CString err = SUCC;
	err=login(mhost,port, muser, mpwd, mdb, bl);
	return err;
}

CString CMysql::login(CString h, int port, CString u, CString p, CString d,bool bl)
{
	CString err=SUCC;
	if (state != 0)
	{
		close();
	}
	if (pcon == NULL)
	{
		if (!init())
		{
			err = geterr();
			return err;
		}
	}
	mck = bl;
	mhost = h;
	muser = u;
	mpwd = p;
	mdb = d;
	string host = UnicodeToUtf8(h.GetBuffer()).data();
	string user = UnicodeToUtf8(u.GetBuffer()).data();
	string pwd = UnicodeToUtf8(p.GetBuffer()).data();
	string db = UnicodeToUtf8(d.GetBuffer()).data();
	
	if (mysql_real_connect(pcon, host.c_str(), user.c_str(), pwd.c_str(), "", port, NULL, 0) == NULL)
	{
		err = geterr();
		return err;
	}
	state = 1;
	CString cs;
	vector<vector<CString>> vec;
	if (bl)
	{
		if (!mdb.IsEmpty())
		{
			err=createdatabase();
			if (err != SUCC)
			{
				return err;
			}
			err=usedb(d);
			if (err != SUCC)
			{
				return err;
			}
			err = createtatle();
			if (err != SUCC)
			{
				return err;
			}
			state = 2;
		}
		else
		{
			return _T("输入数据库");
		}
	}
	else
	{
		err=usedb(d);
	}

	if (err != SUCC)
	{
		close();
	}
	if (err == SUCC)
	{
		saveini(mhost,port,muser, mpwd,mdb);
	}
	return err;
}
CString CMysql::usedb(CString db)
{
	CString sql;
	sql.Format(_T("use  %s;"), db);
	CString err = excutecmd(sql);
	return err;
}
CString CMysql::createdatabase()
{
	CString sql;
	sql.Format(_T("create database if not exists %s ;"), mdb);
	CString err= excutecmd(sql);
	return err;
}
CString CMysql::createtatle()
{
	CString err1 = excutecmd(USER);  //创建用户表
	CString err = SUCC;
	if (err1 != SUCC) //创建失败
	{
		err = err1;
	}
	else
	{

	}
	CString err2 = excutecmd(FRIEND);  //创建用户表
	if (err2 != SUCC) //创建失败
	{
		err = err2;
	}
	return err;
}

CString CMysql::finduser(CString pact, vector<vector<CString>>&res)
{
	CString sql; 
	sql.Format(_T("select * from cuser where act='%s';"), pact);
	CString err = excutecmd(sql, res);
	return err;
}



CString CMysql::addfriend(int id1, int id2)
{
	CString err = SUCC;
	COleDateTime t = COleDateTime::GetCurrentTime();
	CString strt = t.Format(_T("%Y-%m-%d %H:%M:%S"));

	CString sql;
	sql.Format(_T("insert into friend values(%d,%d,'%s');"),
		id1, id2, strt); //保存用户信息
	err = excutecmd(sql);
	if (err != SUCC)
	{
		err = _T("添加失败 ") + err;
	}
	return err;
}
CString CMysql::delfriend(int id1, int id2)
{
	CString err = SUCC;
	CString sql;
	sql.Format(_T("delete from friend where (uid1=%d and uid2=%d) or(uid1=%d and uid2=%d);"),
		id1, id2, id2, id1); //保存用户信息
	err = excutecmd(sql);
	if (err != SUCC)
	{
		err = _T("注册失败 ") + err;
	}
	return err;
}
CString CMysql::reg(vector<CString>&vec)
{
	CString err = SUCC;
	if (vec.size() >= 4)
	{
		vector<vector<CString>>res;
		err = finduser(vec[0], res); //按用户名查询
		if (res.size() > 0)
		{
			return _T("用户名存在");
		}
		//添加注册用户
		CString sql;
		sql.Format(_T("insert into cuser values( null,'%s','%s','%s','%s');"),
			vec[0], vec[1], vec[2], vec[3]); //保存用户信息
		err = excutecmd(sql);
		if (err != SUCC)
		{
			err = _T("注册失败 ") + err;
		}
		return err;
	}
	else
	{
		return _T("数据错误");
	}
}

CString CMysql::login(vector<CString>&vec, vector<vector<CString>>&res)
{
	if (vec.size() >= 2)
	{
		//用户名密码类型正确
		CString sql;
		sql.Format(_T("select * from cuser where act='%s' and pwd='%s';"),vec[0], vec[1]);
		CString err = excutecmd(sql, res);
		if (err == SUCC) 
		{
			if (res.size() <1) //用户不存在
			{
				err = _T("用户名或密码错误");
			}
		}
		else
		{
			err = _T("登录失败 ") + err;
		}
		return err;
	}
	else
	{
		return _T("数据错误");
	}
}




CString CMysql::getfriends(vector<Friend> &records)  //传感器数据查询
{
	records.clear();
	CString err = SUCC;
	if (pcon == NULL)
	{
		err = _T("null");
		return err;
	}
	CString sql=_T("select * from friend;");
	string str = UnicodeToUtf8(sql.GetBuffer()).data();
	if (mysql_query(pcon, str.c_str()) != 0)
	{
		err = geterr();
	}
	else
	{

		MYSQL_RES *res = mysql_store_result(pcon);  //查询结果

		MYSQL_ROW row;
		if (res != NULL)
		{
			CString col;
			string cs;
			while ((row = mysql_fetch_row(res)) != NULL)	//每条记录
			{
				Friend d;  //传感器数据
				d.uid1 = atoi(row[0]); 
				d.uid2 = atoi(row[1]);
				d.strtime = AsciiToUnicode(row[2]).data();
				records.push_back(d);   //保存记录

			}
			mysql_free_result(res);
		}
		else
		{
			err = geterr();
			if (err.IsEmpty())
			{
				err = SUCC;
			}
		}
	}
	return err;
}

CString CMysql::getusers(vector<vector<CString>>&res)
{
	//用户名密码类型正确
	CString sql;
	sql.Format(_T("select * from cuser;"));
	CString err = excutecmd(sql, res);
	return err;
}
