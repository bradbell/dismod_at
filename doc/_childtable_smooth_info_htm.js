// Child table for section smooth_info
document.write('\
<select onchange="smooth_info_child(this)">\
<option>smooth_info-&gt;</option>\
<option>smooth_info_xam.cpp</option>\
</select>\
');
function smooth_info_child(item)
{	var child_list = [
		'smooth_info_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
