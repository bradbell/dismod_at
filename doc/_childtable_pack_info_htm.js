// Child table for section pack_info
document.write('\
<select onchange="pack_info_child(this)">\
<option>pack_info-&gt;</option>\
<option>pack_info_ctor</option>\
<option>pack_info_mulstd</option>\
<option>pack_info_rate_info</option>\
<option>pack_info_mulcov_meas</option>\
<option>pack_info_mulcov_rate</option>\
<option>pack_info_xam.cpp</option>\
</select>\
');
function pack_info_child(item)
{	var child_list = [
		'pack_info_ctor.htm',
		'pack_info_mulstd.htm',
		'pack_info_rate_info.htm',
		'pack_info_mulcov_meas.htm',
		'pack_info_mulcov_rate.htm',
		'pack_info_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
