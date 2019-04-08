// Child table for section split_space
document.write('\
<select onchange="split_space_child(this)">\
<option>split_space-&gt;</option>\
<option>split_space_xam.cpp</option>\
</select>\
');
function split_space_child(item)
{	var child_list = [
		'split_space_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
