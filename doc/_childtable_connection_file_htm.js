// Child table for section connection_file
document.write('\
<select onchange="connection_file_child(this)">\
<option>connection_file-&gt;</option>\
<option>user_connection_file.py</option>\
</select>\
');
function connection_file_child(item)
{	var child_list = [
		'user_connection_file.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
