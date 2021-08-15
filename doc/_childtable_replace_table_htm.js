// Child table for section replace_table
document.write('\
<select onchange="replace_table_child(this)">\
<option>replace_table-&gt;</option>\
<option>replace_table.py</option>\
</select>\
');
function replace_table_child(item)
{	var child_list = [
		'replace_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
