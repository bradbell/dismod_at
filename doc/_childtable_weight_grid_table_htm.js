// Child table for section weight_grid_table
document.write('\
<select onchange="weight_grid_table_child(this)">\
<option>weight_grid_table-&gt;</option>\
<option>weight_grid_table.py</option>\
</select>\
');
function weight_grid_table_child(item)
{	var child_list = [
		'weight_grid_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
