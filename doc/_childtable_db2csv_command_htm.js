// Child table for section db2csv_command
document.write('\
<select onchange="db2csv_command_child(this)">\
<option>db2csv_command-&gt;</option>\
<option>db2csv_command.py</option>\
<option>ihme_db.sh</option>\
</select>\
');
function db2csv_command_child(item)
{	var child_list = [
		'db2csv_command.py.htm',
		'ihme_db.sh.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
