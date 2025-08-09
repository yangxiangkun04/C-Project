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
    /*stmt.prepare("\
        update girls set name='光军',weight=66.9,btime=to_date('2008-01-02 12:12:12','yyyy-mm-dd hh24:mi:ss') where id=10");
    */
   
    //绑定的变量一般用结构体
    struct st_girl
    {
        long id;         //超女编号
        char name[31];   //超女姓名
        double weight;   //超女体重
        char btime[20];  //报名时间
        char memo[301];  //备注，用char[301]对应Oracle的varchar2(300)
    }stgirl;

    //动态sql语句
    stmt.prepare("\
        update girls set name=:1,weight=:2,btime=to_date(:3,'yyyy-mm-dd hh24:mi:ss') where id=:4");
    stmt.bindin(1,stgirl.name,30);
    stmt.bindin(2,stgirl.weight);
    stmt.bindin(3,stgirl.btime,19);
    stmt.bindin(4,stgirl.id);

    //初始化结构体，为变量赋值
    memset(&stgirl,0,sizeof(struct st_girl));
    stgirl.id=11;
    sprintf(stgirl.name,"鹏辉");
    stgirl.weight=66.6;
    sprintf(stgirl.btime,"2021-8-25 10:12:35");

    //执行sql语句，一定要判断返回值，0-成功，其他失败
    //失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中
    if(stmt.execute()!=0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message());
        return -1;
    }

    printf("成功的修改了%ld条记录。\n",stmt.rpc());  //stmt.rpc()是本次执行sql影响的记录数

    
    conn.commit();  //提交事务

    return 0;
}