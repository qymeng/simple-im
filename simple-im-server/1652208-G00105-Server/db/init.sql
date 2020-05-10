--����Server�˵����ݿ��ʼ���ļ�
DROP DATABASE IF EXISTS db1652208;
CREATE DATABASE db1652208;
USE db1652208;

--����������
--<1>�洢�˺���Ϣ	���ܺ󳤶ȹ̶�Ϊ32��������һ�仰��û��ע�ͣ�
DROP TABLE IF EXISTS account;
CREATE TABLE account(
num TINYINT NOT NULL,
name VARCHAR(16) PRIMARY KEY,
password CHAR(32) NOT NULL,
update_password TINYINT DEFAULT 0,
is_online TINYINT DEFAULT 0
);

INSERT INTO account VALUES(1, '����', '111111', 1, 0);
INSERT INTO account VALUES(2, '����', '123456', 1, 0);
INSERT INTO account VALUES(3, '����', 'abcedf', 0, 0);
INSERT INTO account VALUES(4, '����', 'server', 0, 0);

--����
UPDATE account SET password = md5(password);

--<2>�����¼(΢�Ÿ���Ӣ�￴�˿���chat history)
DROP TABLE IF EXISTS chat_history;
CREATE TABLE chat_history(
send_time DATETIME,
sender TINYINT REFERENCES account(num),
receiver TINYINT REFERENCES account(num),
content VARCHAR(1020),
PRIMARY KEY(send_time, sender, receiver)
);

INSERT INTO chat_history VALUES('2018-12-23 01:23:44', 1, 2, '��ã�����');                   
INSERT INTO chat_history VALUES('2018-12-23 01:23:52', 2, 1, '�úú�');                       
INSERT INTO chat_history VALUES('2018-12-23 01:24:07', 1, 2, '�����������Ҳ������');       
INSERT INTO chat_history VALUES('2018-12-23 01:24:23', 2, 1, '�������е�����');               
INSERT INTO chat_history VALUES('2018-12-23 01:25:16', 1, 2, '����Ҫ�ļ�ëǮ�ģ�');           
INSERT INTO chat_history VALUES('2018-12-23 01:25:21', 1, 2, '��ë��');                       
INSERT INTO chat_history VALUES('2018-12-23 01:25:30', 2, 1, '�ҿ�������û�');               
INSERT INTO chat_history VALUES('2018-12-23 01:25:34', 2, 1, '���������');                   
INSERT INTO chat_history VALUES('2018-12-23 01:25:39', 1, 2, '�õ�');                         
INSERT INTO chat_history VALUES('2018-12-23 01:25:42', 2, 1, '886');                          
INSERT INTO chat_history VALUES('2018-12-23 01:27:01', 3, 1, '�������û�����');               
INSERT INTO chat_history VALUES('2018-12-23 01:27:21', 1, 3, '������������');                 
INSERT INTO chat_history VALUES('2018-12-23 01:27:29', 1, 3, '������');                       
INSERT INTO chat_history VALUES('2018-12-23 01:27:38', 3, 1, '����');                         
INSERT INTO chat_history VALUES('2018-12-23 01:27:55', 1, 3, '��ʲô÷��');                   
INSERT INTO chat_history VALUES('2018-12-23 01:27:59', 1, 3, '��÷��');                     
INSERT INTO chat_history VALUES('2018-12-23 01:28:09', 3, 1, '�������������¼Ҳ��������'); 
INSERT INTO chat_history VALUES('2018-12-23 01:28:20', 1, 3, '��ô����ʷ��¼');               
INSERT INTO chat_history VALUES('2018-12-23 01:28:21', 3, 1, '�е�ţ��');                     
INSERT INTO chat_history VALUES('2018-12-23 01:28:44', 3, 1, '���˳�Ȼ�����µ�¼');           
INSERT INTO chat_history VALUES('2018-12-23 01:28:50', 3, 1, '��������Ȼ���');               
INSERT INTO chat_history VALUES('2018-12-23 01:28:53', 1, 3, '��Ҫ�˳���');                   
INSERT INTO chat_history VALUES('2018-12-23 01:29:05', 3, 1, '����Ӧ��Ҳ����');               
INSERT INTO chat_history VALUES('2018-12-23 01:29:11', 1, 3, '���ܲ��Լ�����');               
INSERT INTO chat_history VALUES('2018-12-23 01:29:13', 3, 1, '�˳����Ե�');                   

--<3>�ͻ��˵���������
DROP TABLE IF EXISTS settings;
CREATE TABLE settings(
num TINYINT PRIMARY KEY,
review_amount TINYINT DEFAULT 20
);

INSERT INTO settings VALUES(1, 10);
INSERT INTO settings VALUES(2, 10);
INSERT INTO settings VALUES(3, 10);
INSERT INTO settings VALUES(4, 10);
