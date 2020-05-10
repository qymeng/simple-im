--这是Server端的数据库初始化文件
DROP DATABASE IF EXISTS db1652208;
CREATE DATABASE db1652208;
USE db1652208;

--创建基本表
--<1>存储账号信息	加密后长度固定为32（下面是一句话，没法注释）
DROP TABLE IF EXISTS account;
CREATE TABLE account(
num TINYINT NOT NULL,
name VARCHAR(16) PRIMARY KEY,
password CHAR(32) NOT NULL,
update_password TINYINT DEFAULT 0,
is_online TINYINT DEFAULT 0
);

INSERT INTO account VALUES(1, '李勇', '111111', 1, 0);
INSERT INTO account VALUES(2, '刘晨', '123456', 1, 0);
INSERT INTO account VALUES(3, '王敏', 'abcedf', 0, 0);
INSERT INTO account VALUES(4, '张立', 'server', 0, 0);

--加密
UPDATE account SET password = md5(password);

--<2>聊天记录(微信改了英语看了看叫chat history)
DROP TABLE IF EXISTS chat_history;
CREATE TABLE chat_history(
send_time DATETIME,
sender TINYINT REFERENCES account(num),
receiver TINYINT REFERENCES account(num),
content VARCHAR(1020),
PRIMARY KEY(send_time, sender, receiver)
);

INSERT INTO chat_history VALUES('2018-12-23 01:23:44', 1, 2, '你好，刘晨');                   
INSERT INTO chat_history VALUES('2018-12-23 01:23:52', 2, 1, '好好好');                       
INSERT INTO chat_history VALUES('2018-12-23 01:24:07', 1, 2, '这个聊天框里的也乱码吗');       
INSERT INTO chat_history VALUES('2018-12-23 01:24:23', 2, 1, '好像是有点问题');               
INSERT INTO chat_history VALUES('2018-12-23 01:25:16', 1, 2, '我们要聊几毛钱的？');           
INSERT INTO chat_history VALUES('2018-12-23 01:25:21', 1, 2, '五毛？');                       
INSERT INTO chat_history VALUES('2018-12-23 01:25:30', 2, 1, '我看看别的用户');               
INSERT INTO chat_history VALUES('2018-12-23 01:25:34', 2, 1, '我这就下了');                   
INSERT INTO chat_history VALUES('2018-12-23 01:25:39', 1, 2, '好的');                         
INSERT INTO chat_history VALUES('2018-12-23 01:25:42', 2, 1, '886');                          
INSERT INTO chat_history VALUES('2018-12-23 01:27:01', 3, 1, '我是新用户王敏');               
INSERT INTO chat_history VALUES('2018-12-23 01:27:21', 1, 3, '王敏？网名？');                 
INSERT INTO chat_history VALUES('2018-12-23 01:27:29', 1, 3, '往铭？');                       
INSERT INTO chat_history VALUES('2018-12-23 01:27:38', 3, 1, '王鸣');                         
INSERT INTO chat_history VALUES('2018-12-23 01:27:55', 1, 3, '马什么梅？');                   
INSERT INTO chat_history VALUES('2018-12-23 01:27:59', 1, 3, '马冬梅？');                     
INSERT INTO chat_history VALUES('2018-12-23 01:28:09', 3, 1, '这个改完密码后登录也很流畅了'); 
INSERT INTO chat_history VALUES('2018-12-23 01:28:20', 1, 3, '怎么查历史记录');               
INSERT INTO chat_history VALUES('2018-12-23 01:28:21', 3, 1, '有点牛批');                     
INSERT INTO chat_history VALUES('2018-12-23 01:28:44', 3, 1, '你退出然后重新登录');           
INSERT INTO chat_history VALUES('2018-12-23 01:28:50', 3, 1, '点王敏，然后查');               
INSERT INTO chat_history VALUES('2018-12-23 01:28:53', 1, 3, '还要退出？');                   
INSERT INTO chat_history VALUES('2018-12-23 01:29:05', 3, 1, '现在应该也可以');               
INSERT INTO chat_history VALUES('2018-12-23 01:29:11', 1, 3, '我能查自己的吗');               
INSERT INTO chat_history VALUES('2018-12-23 01:29:13', 3, 1, '退出明显点');                   

--<3>客户端的其他设置
DROP TABLE IF EXISTS settings;
CREATE TABLE settings(
num TINYINT PRIMARY KEY,
review_amount TINYINT DEFAULT 20
);

INSERT INTO settings VALUES(1, 10);
INSERT INTO settings VALUES(2, 10);
INSERT INTO settings VALUES(3, 10);
INSERT INTO settings VALUES(4, 10);
