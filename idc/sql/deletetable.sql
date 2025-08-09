-- 删除T_ZHOBTMIND表2h之前的数据。
--delete from T_ZHOBTMIND where ddatetime<sysdate-2/24;
--commit;
-- 删除T_ZHOBTMIND1表2h之前的数据。
delete from T_ZHOBTMIND1 where ddatetime<sysdate-2/24;
commit;
exit;
