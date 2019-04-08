// Child table for section time_table
document.write('\
<select onchange="time_table_child(this)">\
<option>time_table-&gt;</option>\
<option>time_table.py</option>\
</select>\
');
function time_table_child(item)
{	var child_list = [
		'time_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
