// Child table for section data_subset
document.write('\
<select onchange="data_subset_child(this)">\
<option>data_subset-&gt;</option>\
<option>data_subset_xam.cpp</option>\
</select>\
');
function data_subset_child(item)
{	var child_list = [
		'data_subset_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
