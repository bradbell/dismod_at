// Child table for section create_table
document.write('\
<select onchange="create_table_child(this)">\
<option>create_table-&gt;</option>\
<option>create_table.py</option>\
</select>\
');
function create_table_child(item)
{	var child_list = [
		'create_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
