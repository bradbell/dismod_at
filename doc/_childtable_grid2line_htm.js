// Child table for section grid2line
document.write('\
<select onchange="grid2line_child(this)">\
<option>grid2line-&gt;</option>\
<option>grid2line_xam.cpp</option>\
</select>\
');
function grid2line_child(item)
{	var child_list = [
		'grid2line_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
