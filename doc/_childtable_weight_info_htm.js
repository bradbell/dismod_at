// Child table for section weight_info
document.write('\
<select onchange="weight_info_child(this)">\
<option>weight_info-&gt;</option>\
<option>weight_info_xam.cpp</option>\
</select>\
');
function weight_info_child(item)
{	var child_list = [
		'weight_info_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
