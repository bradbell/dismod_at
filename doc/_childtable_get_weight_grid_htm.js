// Child table for section get_weight_grid
document.write('\
<select onchange="get_weight_grid_child(this)">\
<option>get_weight_grid-&gt;</option>\
<option>get_weight_grid_xam.cpp</option>\
</select>\
');
function get_weight_grid_child(item)
{	var child_list = [
		'get_weight_grid_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
