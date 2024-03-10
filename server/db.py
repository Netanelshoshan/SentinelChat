from constants import *
import sqlite3
import uuid
import struct


def createDB():
    clients_table = """CREATE TABLE
                        clients(ID varchar (16) primary key check(length(ID) == 16),
                            userName varchar (255) check(length(userName) < 255),
                            publicKey varchar (160) check (length (publicKey) == 160),
                            lastSeen INTEGER
                        );"""
    messages_table = """create table
                        messages(ID integer primary key check(length(ID) < 4294967295 ),
                            toClient varchar(16),
                            fromClient varchar (16),
                            type varchar (1),
                            content BLOB,
                            foreign key(toClient) references clients(id),
                            foreign key(fromClient) references clients(fromClient)
                        );"""
    with sqlite3.connect('server.db') as conn:
        try:
            conn.executescript(clients_table)
            conn.executescript(messages_table)
        except Exception:
            conn.close()
            raise Exception
    conn.close()


"""
checks if given name already in the db
"""


def isUsernameAvailable(name):
    query = "select * from clients where userName = ?"
    usersList = []
    with sqlite3.connect('server.db') as conn:
        cur = conn.cursor()
        cur.execute(query, [name])
        usersList = cur.fetchall()
    conn.close()
    if usersList:
        raise Exception


"""
insert new client to the db.
return new uuid
"""


def insertClient(name, key):
    uid = uuid.uuid1().bytes
    query = """insert into clients values (?, ?, ?, datetime('now')) """

    with sqlite3.connect('server.db') as conn:
        cur = conn.cursor()
        # will raise exception if user name already in list OR if id is taken
        try:
            isUsernameAvailable(name)
            cur.execute(query, [uid, name, key])
        except:
            conn.close()
            print('Username isn\'t available')
            raise Exception
    conn.close()
    return uid


"""
returns a byte representation of username, with \0 padding.
"""


def nameToBytes(name):
    tmp = bytes(name, 'utf-8')
    return bytearray(tmp + (USERNAME_SIZE - len(tmp)) * bytes('\0', 'utf-8'))


"""
returns userlist excluding the calling user
 """


def getUsersList(uid):
    query = "select * from clients where id <> ?"
    with sqlite3.connect('server.db') as conn:
        cur = conn.cursor()
        cur.execute(query, [uid])
        usersList = cur.fetchall()
    conn.close()
    ret = bytes()
    for user in usersList:
        ret = ret + user[0] + nameToBytes(user[1])
    return ret


"""
 returns uid's public key
"""


def getUserPubKey(uid):
    query = "select PublicKey from clients where id = ?"
    pubKey = ''

    with sqlite3.connect('server.db') as conn:
        cur = conn.cursor()
        cur.execute(query, [uid])
        pubKey = cur.fetchall()
    conn.close()
    return pubKey[0][0]


"""
saves the message in db
"""


def saveMsg(toClient, fromClient, msgType, content):
    query = """insert into messages values (?, ?, ?, ?, ?) """

    with sqlite3.connect('server.db') as conn:
        cur = conn.cursor()
        try:
            cur.execute(query, [None, toClient, fromClient, msgType, content])
            return cur.lastrowid
        except Exception:
            raise Exception

    conn.close()


"""
deletes all the messages for given client uid.
"""


def deleteMessages(uid):
    query = """delete from messages where ToClient = ?"""

    with sqlite3.connect('server.db') as conn:
        cur = conn.cursor()
        cur.execute(query, [uid])
    conn.close()


"""
Gets pending (unread) messages from the db to the client.
"""


def getMessages(uid):
    query = """select * from messages where ToClient = ?"""
    with sqlite3.connect('server.db') as conn:
        cur = conn.cursor()
        cur.execute(query, [uid])
        messages = cur.fetchall()
    conn.close()

    # delete "unread" messages after fetching them from the db.
    deleteMessages(uid)

    content = bytes()
    if messages:
        for msg in messages:
            content += msg[2] + \
                       struct.pack('<I', msg[0]) + \
                       msg[3] + \
                       struct.pack('<I', len(msg[4])) + \
                       msg[4]
    return content


"""
Update LastSeen attribute in client's table.
"""


def updateConnTime(uid):
    query = "update clients set lastSeen = datetime('now') where id = ?"
    with sqlite3.connect('server.db') as conn:
        cur = conn.cursor()
        cur.execute(query, [uid])
    conn.close()
