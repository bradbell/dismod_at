# include <stdio.h>
# include <sqlite3.h>


const char* get_table_column_type(
	sqlite3*    db          ,
	const char* table_name  ,
	const char* column_name )
{	// check the type for this column
	const char *zDataType;
	const char *zCollSeq;
	int NotNull;
	int PrimaryKey;
	int Autoinc;
	rc = sqlite3_table_column_metadata(
		db,
		"main", 
		"mytable", 
		column_name[j], 
		&zDataType, 
		&zCollSeq, 
		&NotNull, 
		&PrimaryKey, 
		&Autoinc
	);
	if( rc ){
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db) );
		sqlite3_close(db);
		return(1);
	}
	return zCollSeq;
}

int get_table_column_text(     // number of rows
	sqlite3*    db          ,  // database connection
	const char* table_name  ,
	const char* column_name ,
	char***     data        )
{	// check the type for this column
	const char* type = get_table_column_text(db, table_name, table_column);
	assert( strcmp(type, "text") == 0 )
}
