// Child table for section avgint_subset
document.write('\
<select onchange="avgint_subset_child(this)">\
<option>avgint_subset-&gt;</option>\
<option>avgint_subset_xam.cpp</option>\
</select>\
');
function avgint_subset_child(item)
{	var child_list = [
		'avgint_subset_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
