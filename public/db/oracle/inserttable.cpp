//操作Oracle数据库中 向表中插入数据
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

    //准备插入表中的sql语句
    //静态sql语句
    /*
    stmt.prepare("insert into girls(id,name,weight,btime,memo)\
                 values(1,'月月鸟',48.5,to_date('2000-01-01 12:30:35','yyyy-mm-dd hh24:mi:ss'),'月月鸟是个好女孩')");

    //执行sql语句，一定要判断返回值，0-成功。其他失败
    //失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中
    if(stmt.execute()!=0)
    {
        printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message());
        return -1;
    }

    printf("成功的插入了%ld条记录。\n",stmt.rpc());  //stmt.rpc()是本次执行sql影响的记录数
    */

    //1.如果字段是字符串型，绑定的变量可以用char[],也可以用string，推荐用char[]
    //2.如果字段是字符串型，bindin()的第三个参数填字段的长度，太小可能会有问题，不推荐缺省值2000
    //3.动态sql语句的字段也可以填静态的值
    //绑定的变量一般用结构体
    struct st_girl
    {
        long id;         //超女编号
        char name[31];   //超女姓名
        double weight;   //超女体重
        char btime[20];  //报名时间
        char memo[301];  //备注，用char[301]对应Oracle的varchar2(300)
    }stgirl;
    //动态sql语句,适用于多次执行的sql语句
    stmt.prepare("insert into girls(id,name,weight,btime,memo)\
                    values(:1,:2,:3,to_date(:4,'yyyy-mm-dd hh24:mi:ss'),:5)");  //:1,:2...可以理解为输入参数
    stmt.bindin(1,stgirl.id);
    stmt.bindin(2,stgirl.name,30);
    stmt.bindin(3,stgirl.weight);
    stmt.bindin(4,stgirl.btime,19);
    stmt.bindin(5,stgirl.memo,300);

    //对变量赋值，执行sql语句
    for(int ii=10;ii<15;ii++)
    {
        //初始化变量
        memset(&stgirl,0,sizeof(struct st_girl));

        //为变量赋值
        stgirl.id=ii;
        sprintf(stgirl.name,"月月鸟%05dgirl",ii);
        stgirl.weight=86.23+ii;
        sprintf(stgirl.btime,"2025-7-25 2:48:%02d",ii);
        sprintf(stgirl.memo,"这是第%d个好女孩的备注",ii);

        //执行sql语句，一定要判断返回值，0-成功
        //失败代码在stmt.m_cda.rc中，失败描述在stmt.m_cda.message中
        if(stmt.execute()!=0)
        {
            printf("stmt.execute() failed.\n%s\n%s\n",stmt.sql(),stmt.message());
            return -1;
        }

        printf("成功的插入了%ld条记录。\n",stmt.rpc());//stmt.rpc()是本次执行sql影响的记录数

    }
    

    conn.commit();  //提交事务

    return 0;
}