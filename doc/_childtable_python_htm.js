// Child table for section python
document.write('\
<select onchange="python_child(this)">\
<option>python-&gt;</option>\
<option>create_connection</option>\
<option>create_table</option>\
<option>create_database</option>\
<option>csv2db_command</option>\
<option>db2csv_command</option>\
<option>get_name_type</option>\
<option>get_row_list</option>\
<option>get_table_dict</option>\
<option>metropolis</option>\
<option>modify_command</option>\
<option>sql_command</option>\
<option>unicode_tuple</option>\
</select>\
');
function python_child(item)
{	var child_list = [
		'create_connection.htm',
		'create_table.htm',
		'create_database.htm',
		'csv2db_command.htm',
		'db2csv_command.htm',
		'get_name_type.htm',
		'get_row_list.htm',
		'get_table_dict.htm',
		'metropolis.htm',
		'modify_command.htm',
		'sql_command.htm',
		'unicode_tuple.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
