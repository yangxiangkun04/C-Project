//操作Oracle数据库中 更新表
#include "_ooci.h"
using namespace idc;

int main(int argc,char* argv[])
{
    connection conn;  //创建数据库连接的对象

    //登录数据库，成功返回0，失败返回其他值
    //失败代码在conn.m_cda.rc中，失败描述在conn.m_cda.message中
    if(conn.connecttodb("scott/123456@snorcl11g_138","AMERICAN_AMERICA.AL32UTF8")!=0)
    {
        printf("connecttodb failed.\n%d,%s\n",conn.rc(),conn.message());
        return -1;
    }

    printf("connect database ok.\n");

    sqlstatement stmt;  //操作sql语句的对象
    stmt.connect(&conn);  //指定stmt对象使用的数据库连接

    //静态sql语句
    stmt.prepare("delete from girls where id=10");
    
    //执行sql语句，一定要判断返回值，0-成功，其他失败
    //失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中
    if(stmt.execute()!=0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message());
        return -1;
    }

    printf("成功的删除了%ld条记录。\n",stmt.rpc());  //stmt.rpc()是本次执行sql影响的记录数
   
    int minid=11,maxid=13;

    //动态sql语句
    stmt.prepare("delete from girls where id>=:1 and id <=:2");
    stmt.bindin(1,minid);
    stmt.bindin(2,maxid);

    //执行sql语句，一定要判断返回值，0-成功，其他失败
    //失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中
    if(stmt.execute()!=0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message());
        return -1;
    }

    printf("成功的删除了%ld条记录。\n",stmt.rpc());  //stmt.rpc()是本次执行sql影响的记录数

    
    conn.commit();  //提交事务

    return 0;
}