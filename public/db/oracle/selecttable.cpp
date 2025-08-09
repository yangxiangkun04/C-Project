//操作Oracle数据库中 查询表中的数据
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

    int minid=1,maxid=13;
    struct st_girl
    {
        long id;         //超女编号
        char name[31];   //超女姓名
        double weight;   //超女体重
        char btime[20];  //报名时间
        char memo[301];  //备注，用char[301]对应Oracle的varchar2(300)
    }stgirl;

    //准备查询表的sql语句，prepare()方法不需要判断返回值
    stmt.prepare("select id,name,weight,to_char(btime,'yyyy-mm-dd hh24:mi:ss'),memo from girls where id>=:1 and id <= :2");
    stmt.bindin(1,minid);
    stmt.bindin(2,maxid);
    //把查询语句的结果集与变量的地址绑定，bindout()方法不需要判断返回值
    stmt.bindout(1,stgirl.id);
    stmt.bindout(2,stgirl.name,30);
    stmt.bindout(3,stgirl.weight);
    stmt.bindout(4,stgirl.btime,19);
    stmt.bindout(5,stgirl.memo,300);

    //执行sql语句，一定要判断返回值，0-成功，其他失败
    //失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中
    if(stmt.execute()!=0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message());
        return -1;
    }

    //本程序执行的是查询语句，执行stmt.execute()后，将在数据库的缓冲区中产生一个结果集
    while(1)
    {
        memset(&stgirl,0,sizeof(stgirl));  //先把结构体初始化

        //从结果集中获取一条记录，一定要判断返回值，0-成功，1403-无记录，其他-失败
        //在实际开发中，除了0和1403，其他的情况极少出现
        if(stmt.next()!=0) break;

        //把获取到的记录的值打印出来
        printf("id=%ld,name=%s,weight=%.02f,btime=%s,memo=%s\n",stgirl.id,stgirl.name,stgirl.weight,stgirl.btime,stgirl.memo);

    }
    
    //请注意，stmt.m_cda.rpc变量非常重要，它保存了sql被执行后影响的记录数
    printf("本次查询到了girl表%ld条记录。\n",stmt.rpc());

    return 0;
}