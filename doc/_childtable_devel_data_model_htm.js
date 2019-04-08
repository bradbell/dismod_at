// Child table for section devel_data_model
document.write('\
<select onchange="devel_data_model_child(this)">\
<option>devel_data_model-&gt;</option>\
<option>data_model_ctor</option>\
<option>data_model_replace_like</option>\
<option>data_model_average</option>\
<option>data_model_like_one</option>\
<option>data_model_like_all</option>\
</select>\
');
function devel_data_model_child(item)
{	var child_list = [
		'data_model_ctor.htm',
		'data_model_replace_like.htm',
		'data_model_average.htm',
		'data_model_like_one.htm',
		'data_model_like_all.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
