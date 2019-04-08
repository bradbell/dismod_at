// Child table for section density_table
document.write('\
<select onchange="density_table_child(this)">\
<option>density_table-&gt;</option>\
<option>density_table.py</option>\
</select>\
');
function density_table_child(item)
{	var child_list = [
		'density_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
