#include <windows.h>
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <vector>
#include <string>
using namespace std;

class ODBC
{
private:
	SQLHANDLE hEnv;
	SQLHANDLE hDbc;
	SQLHANDLE hStmt;
	SQLRETURN retCode;
	SQLINTEGER retErro;
	SQLINTEGER rowCount;
	SQLSMALLINT colCount;
	bool bState;
	char* pszUName;
	char* pszUPassword;
	char* pszDSN;
public:
	ODBC();
	~ODBC();
	bool Connect();
	bool Close();
	bool IsOpen();
	int GetRowCount(){return rowCount;}
	int GetColCount(){return colCount;}
	int ExecuteQuery(const char* pszSql);  
	int ExecuteNonQuery(const char* pszSql);
	int ExecuteUpdate(const char* pszSql); 
	vector<string*> ExecuteQueryVector(const char* pszSql);  
};