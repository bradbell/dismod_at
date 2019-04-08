// Child table for section age_avg_grid
document.write('\
<select onchange="age_avg_grid_child(this)">\
<option>age_avg_grid-&gt;</option>\
<option>age_avg_grid_xam.cpp</option>\
</select>\
');
function age_avg_grid_child(item)
{	var child_list = [
		'age_avg_grid_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
