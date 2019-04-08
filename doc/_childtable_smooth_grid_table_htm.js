// Child table for section smooth_grid_table
document.write('\
<select onchange="smooth_grid_table_child(this)">\
<option>smooth_grid_table-&gt;</option>\
<option>smooth_grid_table.py</option>\
</select>\
');
function smooth_grid_table_child(item)
{	var child_list = [
		'smooth_grid_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
