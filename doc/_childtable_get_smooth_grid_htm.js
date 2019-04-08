// Child table for section get_smooth_grid
document.write('\
<select onchange="get_smooth_grid_child(this)">\
<option>get_smooth_grid-&gt;</option>\
<option>get_smooth_grid_xam.cpp</option>\
</select>\
');
function get_smooth_grid_child(item)
{	var child_list = [
		'get_smooth_grid_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
