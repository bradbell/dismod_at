// Child table for section data_model_like_one
document.write('\
<select onchange="data_model_like_one_child(this)">\
<option>data_model_like_one-&gt;</option>\
<option>like_one_xam.cpp</option>\
</select>\
');
function data_model_like_one_child(item)
{	var child_list = [
		'like_one_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
