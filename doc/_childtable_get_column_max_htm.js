// Child table for section get_column_max
document.write('\
<select onchange="get_column_max_child(this)">\
<option>get_column_max-&gt;</option>\
<option>get_column_max_xam.cpp</option>\
</select>\
');
function get_column_max_child(item)
{	var child_list = [
		'get_column_max_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
