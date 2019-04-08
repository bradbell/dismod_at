// Child table for section node_table
document.write('\
<select onchange="node_table_child(this)">\
<option>node_table-&gt;</option>\
<option>node_table.py</option>\
</select>\
');
function node_table_child(item)
{	var child_list = [
		'node_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
