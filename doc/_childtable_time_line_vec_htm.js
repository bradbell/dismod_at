// Child table for section time_line_vec
document.write('\
<select onchange="time_line_vec_child(this)">\
<option>time_line_vec-&gt;</option>\
<option>time_line_vec_xam.cpp</option>\
</select>\
');
function time_line_vec_child(item)
{	var child_list = [
		'time_line_vec_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
