#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ "$0" != 'bin/example_c.sh' ]
then
	echo 'bin/example_c.sh: must be executed from its parent directory.'
	exit 1
fi
# -----------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# ------------------------------------------------------------------------
if [ -e junk ]
then
	echo_eval rm -r junk
fi
echo_eval mkdir junk
echo_eval cd junk
# ------------------------------------------------------------------------
# create a database and a table 
cat << EOF > example.sql
create table mytable(one text, two int);
insert into mytable values('hello',   1);
insert into mytable values('goodbye', 2);
select * from mytable;
EOF
echo_eval sqlite3 example.db < example.sql
# ------------------------------------------------------------------------
# program that executes one sql statement
cat << EOF > example.c
# include <stdio.h>
#include <sqlite3.h>

# if 0
int sqlite3_table_column_metadata(
  sqlite3 *db,                /* Connection handle */
  const char *zDbName,        /* Database name or NULL */
  const char *zTableName,     /* Table name */
  const char *zColumnName,    /* Column name */
  char const **pzDataType,    /* OUTPUT: Declared data type */
  char const **pzCollSeq,     /* OUTPUT: Collation sequence name */
  int *pNotNull,              /* OUTPUT: True if NOT NULL constraint exists */
  int *pPrimaryKey,           /* OUTPUT: True if column part of PK */
  int *pAutoinc               /* OUTPUT: True if column is auto-increment */
);
# endif

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

int main(int argc, char **argv){
  sqlite3* db;
  char*    zErrMsg = 0;
  int      rc;
  int      j;

  rc = sqlite3_open("example.db", &db);
  if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return(1);
  }
# if 0
  // For some unknown reason: this request generates the erorr message 
  //  SQL error: no such table column: mytable.one
  const char *column_name[] = { "one", "two" };
  for(j = 0; j < 2; j++)
  {  const char *zDataType;
     const char *zCollSeq;
     int NotNull;
     int PrimaryKey;
     int Autoinc;
     rc = sqlite3_table_column_metadata(
      db,
      "example", 
      "mytable", 
      column_name[j], 
      &zDataType, 
      &zCollSeq, 
      &NotNull, 
      &PrimaryKey, 
      &Autoinc
    );
    if( rc ){
      fprintf( stderr, "SQL error: %s\n", sqlite3_errmsg(db) );
      sqlite3_close(db);
      return(1);
    }
    printf("name = %s, type = %s\n", column_name[j], zDataType);
  }
# endif
  rc = sqlite3_exec(db, "select one,two from mytable", callback, 0, &zErrMsg);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  sqlite3_close(db);
  return 0;
}
EOF
echo_eval gcc example.c -lsqlite3 -o example
echo_eval ./example
