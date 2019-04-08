// Child table for section bilinear_interp
document.write('\
<select onchange="bilinear_interp_child(this)">\
<option>bilinear_interp-&gt;</option>\
<option>bilinear_interp_xam.cpp</option>\
</select>\
');
function bilinear_interp_child(item)
{	var child_list = [
		'bilinear_interp_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
