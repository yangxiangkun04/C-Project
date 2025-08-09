//本程序是共享平台的公共功能模块，用于清理表中的历史数据

#include "_public.h"
#include "_ooci.h"
using namespace idc;

struct st_arg
{
    char connstr[101];   // 数据库的连接参数。
    char tname[31];      // 待清理的表名。
    char keycol[31];     // 待清理的表的唯一键字段名。
    char where[1001];    // 待清理的数据需要满足的条件。
    int  maxcount;       // 执行一次SQL删除的记录数。
    char starttime[31];  // 程序运行的时间区间。
    int  timeout;        // 本程序运行时的超时时间。
    char pname[51];      // 本程序运行时的程序名。
} starg;

void _help();
void EXIT(int sig);
// 把xml解析到参数starg结构中
bool _xmltoarg(const char *strxmlbuffer);
// 判断当前时间是否在程序运行的时间区间内。
bool instarttime();
// 业务处理主函数。
bool _deletetable();

clogfile logfile;
connection conn;

cpactive pactive;

int main(int argc,char* argv[])
{
    if(argc!=3) {_help(); return -1;}

    //closeioandsignal(); 
    signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

    if(logfile.open(argv[1])==false)
    {
        printf("打开日志文件失败（%s）。\n",argv[1]); 
        return -1;
    }

    //把xml解析到starg结构体中
    if(_xmltoarg(argv[2])==false) return -1;

    // 判断当前时间是否在程序运行的时间区间内。
    if(instarttime()==false) return 0;

    if(conn.connecttodb(starg.connstr,"AMERICAN_AMERICA.AL32UTF8",true)!=0)  //第三个参数是是否启用自动提交，启用自动提交
    {
        logfile.write("connecttodb(%s) failed.\n%s\n",starg.connstr,conn.message());
        EXIT(-1);
    }

    //业务处理主函数
    _deletetable();

}

void _help()
{
    printf("Using:/project/tools/bin/deletetable logfilename xmlbuffer\n\n");

    printf("Sample:/project/tools/bin/procctl 3600 /project/tools/bin/deletetable /log/idc/deletetable_ZHOBTMIND1.log "\
                         "\"<connstr>idc/idcpwd@snorcl11g_138</connstr><tname>T_ZHOBTMIND1</tname>"\
                         "<keycol>rowid</keycol><where>where ddatetime<sysdate-0.03</where>"\
                         "<maxcount>10</maxcount><starttime>22,23,00,01,02,03,04,05,06,13</starttime>"\
                         "<timeout>120</timeout><pname>deletetable_ZHOBTMIND1</pname>\"\n\n");

    printf("本程序是共享平台的公共功能模块，用于清理表中的数据。\n");

    printf("logfilename 本程序运行的日志文件。\n");
    printf("xmlbuffer   本程序运行的参数，用xml表示，具体如下：\n\n");

    printf("connstr     数据库的连接参数，格式：username/passwd@tnsname。\n");
    printf("tname       待清理数据表的表名。\n");
    printf("keycol      待清理数据表的唯一键字段名，可以用记录编号，如keyid，建议用rowid，效率最高。\n");
    printf("where       待清理的数据需要满足的条件，即SQL语句中的where部分。\n");
    printf("maxcount    执行一次SQL语句删除的记录数，建议在100-500之间。\n");
    printf("starttime   程序运行的时间区间，例如02,13表示：如果程序运行时，踏中02时和13时则运行，其它时间不运行。"\
                                "如果starttime为空，本参数将失效，只要本程序启动就会执行数据清理，"\
                                "为了减少对数据库的压力，数据清理一般在业务最闲的时候时进行。\n");
    printf("timeout     本程序的超时时间，单位：秒，建议设置120以上。\n");
    printf("pname       进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n");
}

bool _xmltoarg(const char* strxmlbuffer)
{
    memset(&starg,0,sizeof(struct st_arg));

    getxmlbuffer(strxmlbuffer,"connstr",starg.connstr,100);
    if(strlen(starg.connstr)==0){logfile.write("connstr is null.\n"); return false;}

    getxmlbuffer(strxmlbuffer,"tname",starg.tname,30);
    if (strlen(starg.tname)==0) { logfile.write("tname is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"keycol",starg.keycol,30);
    if (strlen(starg.keycol)==0) { logfile.write("keycol is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"where",starg.where,1000);
    if (strlen(starg.where)==0) { logfile.write("where is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"starttime",starg.starttime,30);

    getxmlbuffer(strxmlbuffer,"maxcount",starg.maxcount);
    if (starg.maxcount==0) { logfile.write("maxcount is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"timeout",starg.timeout);
    if (starg.timeout==0) { logfile.write("timeout is null.\n"); return false; }

    getxmlbuffer(strxmlbuffer,"pname",starg.pname,50);
    if (strlen(starg.pname)==0) { logfile.write("pname is null.\n"); return false; }

    return true;

}

void EXIT(int sig)
{
    logfile.write("程序退出，sig=%d\n",sig);

    conn.disconnect();

    exit(0);
}

// 判断当前时间是否在程序运行的时间区间内。
bool instarttime()
{
    // 程序运行的时间区间，例如02,13表示：如果程序启动时，踏中02时和13时则运行，其它时间不运行。
    if (strlen(starg.starttime)!=0)
    {
        string strhh24=ltime1("hh24");  // 获取当前时间的小时，如果当前时间是2023-01-08 12:35:40，将得到12。
        if (strstr(starg.starttime,strhh24.c_str())==0) return false;
    }       // 闲时：12-14时和00-06时。

    return true;
}

// 业务处理主函数。
// 从表中按照某个条件（where）查询记录主键，然后分批删除这些记录，每批最多删除 starg.maxcount 条。
bool _deletetable()
{
    ctimer timer;  //计时器

    char tmpvalue[21];  //存放待删除记录的唯一键的值，准备绑定输出参数

    //1.准备从表中提取数据的sql语句
    //从表 starg.tname 中查询满足 starg.where 条件的主键字段 starg.keycol；查询结果输出到 tmpvalue。
    // select rowid from T_ZHOBTMIND1 where ddatetime<sysdate-1
    sqlstatement stmtsel(&conn);
    stmtsel.prepare("select %s from %s %s",starg.keycol,starg.tname,starg.where);
    stmtsel.bindout(1,tmpvalue,20);

    //2.准备从表中删除数据的sql语句，绑定输入参数
    // delete from T_ZHOBTMIND1 where rowid in (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10);
    string strsql=sformat("delete from %s where %s in (",starg.tname,starg.keycol);
    for(int ii=0;ii<starg.maxcount;ii++)
    {
        strsql=strsql+sformat(":%lu,",ii+1);
    }
    deleterchr(strsql,',');  //最后一个逗号是多余的
    strsql=strsql+")";
    //这部分只是准备了删除语句

    //查询到的数据有很多行
    char keyvalues[starg.maxcount][21];  //存放唯一键字段的值的数组,每一行数据的唯一键值，作为绑定输入参数传入stmtdel语句

    //绑定
    sqlstatement stmtdel(&conn);
    stmtdel.prepare(strsql);
    for(int ii=0;ii<starg.maxcount;ii++)
    {
        stmtdel.bindin(ii+1,keyvalues[ii],20);
    }

    if(stmtsel.execute()!=0)  //执行提取数据的sql语句
    {
        logfile.write("stmtsel.execute() failed.\n%s\n%s\n",stmtdel.sql(),stmtdel.message());
        return false;
    }

    int ccount=0;  //用于记录keyvalues数组中有效元素的个数
    memset(keyvalues,0,sizeof(keyvalues));

    //3.处理结果集
    while(true)
    {
        //a.从结果集中获取一行记录，放在临时的数组中
        memset(tmpvalue,0,sizeof(tmpvalue));
        if(stmtsel.next()!=0) break;  //从结果集中读取一行，自动写入tmpvalue，每次调用.next()，tmpvalue的值就会被填为当前行的主键值
        strcpy(keyvalues[ccount],tmpvalue);  //将tmpvalue中的值传递给keyvalues
        ccount++; 
 
        //b.如果数组中的记录数达到了starg.maxcount,执行一次删除数据的sql语句
        if(ccount==starg.maxcount)
        {
            if(stmtdel.execute()!=0)  //执行从表中删除数据的sql语句
            {
                logfile.write("stmtdel.execute() failed.\n%s\n%s\n",stmtdel.sql(),stmtdel.message());
                return false;
            }

            ccount=0;
            memset(keyvalues,0,sizeof(keyvalues));

            pactive.uptatime();  //进程心跳
        }
    }
    //4.如果临时数组中有记录，再执行一次删除数据的sql语句
    if(ccount>0)
    {
        if(stmtdel.execute()!=0)
        {
            logfile.write("stmtdel.execute() failed.\n%s\n%s\n",stmtdel.sql(),stmtdel.message());
            return false;
        }
    }

    if(stmtsel.rpc()>0)
        logfile.write("delete from %s %d rows in %.02fsec.\n",starg.tname,stmtsel.rpc(),timer.elapsed());

    return true;
}


