// Child table for section pack_prior
document.write('\
<select onchange="pack_prior_child(this)">\
<option>pack_prior-&gt;</option>\
<option>pack_prior_xam.cpp</option>\
</select>\
');
function pack_prior_child(item)
{	var child_list = [
		'pack_prior_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
