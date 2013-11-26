-#include "Database.h"

ODBC::ODBC()
{
    bState=false;
    rowCount=colCount=0;
    retCode=SQLAllocHandle(SQL_HANDLE_ENV,NULL,&hEnv);
    if((retCode!=SQL_SUCCESS)&& (retCode != SQL_SUCCESS_WITH_INFO))
    {
       cout<<"Erro AllocHandle"<<retCode<<endl;
       return;
    }
    retCode=SQLSetEnvAttr(hEnv,SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3,SQL_IS_INTEGER);
    if((retCode!=SQL_SUCCESS)&& (retCode != SQL_SUCCESS_WITH_INFO))
    {
       cout<<"Erro AllocHandle"<<retCode<<endl;
       SQLFreeHandle( SQL_HANDLE_DBC, hEnv );
       return;
    }
    retCode=SQLAllocHandle(SQL_HANDLE_DBC,hEnv,&hDbc);
    if((retCode!=SQL_SUCCESS)&& (retCode != SQL_SUCCESS_WITH_INFO))
    {
       cout<<"Erro AllocHandle"<<retCode<<endl;
       SQLFreeHandle( SQL_HANDLE_DBC, hEnv );
       return;
    }
    pszUName="sa";       
    pszUPassword="123456"; 
    pszDSN="HH";         
   
 
}
ODBC::~ODBC()
{
    Close();
}
bool ODBC::Connect()
{
    if(bState==false)
    {
      retCode=SQLConnect(hDbc,(SQLCHAR*) pszDSN,SQL_NTS,(SQLCHAR*) pszUName,SQL_NTS,(SQLCHAR*) pszUPassword,SQL_NTS);
       if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
       {
           cout<<"Erro Connect "<<retCode<<endl;
           SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
           return false;
       }
       retCode=SQLAllocHandle(SQL_HANDLE_STMT,hDbc,&hStmt);
       if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
       {
           cout<<"Erro Connect "<<retCode<<endl;
           SQLDisconnect( hDbc );
           SQLFreeHandle( SQL_HANDLE_DBC, hDbc);
           return false;
       }
    }     
    bState=true;
    cout<<"success!"<<endl;
    return true;
   
}
int ODBC::ExecuteQuery(const char* pszSql)
{
   
    if(pszSql==NULL)
       return 0;
    cout<<"hStmt="<<hStmt<<endl;
    retCode=SQLExecDirect(hStmt,(SQLCHAR*)pszSql,SQL_NTS);
    if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
    {
       cout<<"Erro ExecDirect "<<retCode<<endl;
       return -1;
    }
/*  retCode=SQLRowCount(hStmt,&rowCount);  
    if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
    {
       cout<<"Erro RowCount "<<retCode<<endl;
       return -1;
    }*/
    retCode=SQLNumResultCols(hStmt,&colCount);
    if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
    {
       cout<<"Erro ColCount "<<retCode<<endl;
       return -1;
    }
    rowCount=0;
    while(SQL_NO_DATA!=SQLFetch(hStmt))
    {
       //cout<<pszBuf<<endl;
       rowCount++;
    }
    SQLCancel(hStmt);
    return rowCount;
}
int ODBC::ExecuteNonQuery(const char* pszSql)
{
    rowCount=0;
    if(pszSql==NULL)
       return 0;
    //cout<<"hStmt="<<hStmt<<endl;
    retCode=SQLExecDirect(hStmt,(SQLCHAR*)pszSql,SQL_NTS);
    if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
    {
       cout<<"Erro ExecDirect "<<retCode<<endl;
       return -1;
    }
    retCode=SQLRowCount(hStmt,&rowCount); 
    if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
    {
       cout<<"Erro RowCount "<<retCode<<endl;
       return -1;
    }
    retCode=SQLNumResultCols(hStmt,&colCount);
    if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
    {
       cout<<"Erro ColCount "<<retCode<<endl;
       return -1;
    }
   
    SQLCancel(hStmt);
    return rowCount;
}

vector<string* >  ODBC::ExecuteQueryVector(const char* pszSql)  
{  
    vector<string* > v;  
    /*if(pszSql==NULL)  
       return ; */ 
    retCode=SQLExecDirect(hStmt,(SQLCHAR*)pszSql,SQL_NTS);  
    if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))  
    {  
       cout<<"Erro ExecDirect "<<retCode<<endl;  
       //return;  
    }  
/*  retCode=SQLRowCount(hStmt,&rowCount);  //不受select 影响。。 
    if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO)) 
    { 
       cout<<"Erro RowCount "<<retCode<<endl; 
       return -1; 
    }*/  
    retCode=SQLNumResultCols(hStmt,&colCount);  
    if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))  
    {  
       cout<<"Erro ColCount "<<retCode<<endl;  
       //return ;  
    }  
    rowCount=0;  
    SQLINTEGER colLen = 0;  
    SQLSMALLINT buf_len = 0;  
    SQLINTEGER colType = 0;  
   
    while(true)  
    {    
       char sz_buf[256];  
       char* pszBuf;  
       SQLINTEGER  buflen;  
       string* rowData=new string[colCount+1];    
       if(SQLFetch(hStmt)==SQL_NO_DATA)  
       {  
           break;  
       }  
       for(int i=1;i<=colCount;i++)  
       {  
           SQLColAttribute(hStmt, i, SQL_DESC_NAME, sz_buf, 256, &buf_len, 0);  
           SQLColAttribute(hStmt, i, SQL_DESC_TYPE, 0, 0, 0, &colType);  
           SQLColAttribute(hStmt, i, SQL_DESC_LENGTH, NULL, 0, 0, &colLen);  
           pszBuf=new char[colLen+1];  
           pszBuf[0]='/0';  
           SQLGetData(hStmt,i,SQL_C_CHAR,pszBuf,50,&buflen);  
           rowData[i-1]=pszBuf;  
       }  
       v.push_back(rowData);     
       rowCount++;  
       //cout<<"rowData[0]="<< (rowData[0])<<" rowData[1]="<<rowData[1]<<" rowData[2]="<<rowData[2]<<endl;  
    }  
    SQLCancel(hStmt);  
    return v;  
}  

bool ODBC::Close()
{
    if(bState)
    {
       SQLDisconnect(hDbc);
       SQLFreeHandle(SQL_HANDLE_DBC,hDbc);
       SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
       bState=false;
    }
 
    return true;
}
bool ODBC::IsOpen()
{
    return bState;
}
 