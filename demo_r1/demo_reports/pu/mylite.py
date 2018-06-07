#! /usr/bin/python
# coding: utf-8

"""Copy table from mysql to sqlite.
Require:
* SQLAlchemy
* MySQLdb or PyMySQL
Usage:
mylite.py "mysql+pymysql://user:password@host/db?charset=utf8" "sqlite:///out.db" table_name [table_name2...]
"""

import sqlalchemy as sa
from sqlalchemy.ext.compiler import compiles
from sqlalchemy.dialects import mysql

@compiles(mysql.TINYINT, 'sqlite')
@compiles(mysql.SMALLINT, 'sqlite')
def compile_sqlite_tinyint(type_, compiler, **kw):
    return 'INTEGER'

@compiles(mysql.LONGBLOB, 'sqlite')
def compile_sqlite_tinyint(type_, compiler, **kw):
    return 'BLOB'


def copy_table(my_engine, lite_engine, table_name):
    meta = sa.MetaData(my_engine)
    table = sa.Table(table_name, meta, autoload=True)

    lite_engine.execute("DROP TABLE IF EXISTS " + table_name)
    table.create(lite_engine)

    rows = my_engine.execute(table.select()).fetchall()
    with lite_engine.begin() as con:
        for row in rows:
            con.execute(table.insert().values(**row))

def main():
    import sys
    mysql = sa.create_engine(sys.argv[1])
    lite  = sa.create_engine(sys.argv[2])
    for table in sys.argv[3:]:
        copy_table(mysql, lite, table)

if __name__ == '__main__':
    main()