<?php

include("geoip.inc");

$gi = geoip_open("GeoIP.dat");

define('QUERY_COUNTRY_CODE', 0);
define('QUERY_COUNTRY_NAME', 1);

switch($_POST['query'])
{
	case QUERY_COUNTRY_CODE:
	{
		if($_POST['ipv6'] == 1) echo geoip_country_code_by_addr_v6($gi, $_POST['ip']);
		else echo geoip_country_code_by_addr($gi, $_POST['ip']);
		break;
	}
	case QUERY_COUNTRY_NAME:
	{
		if($_POST['ipv6'] == 1) echo geoip_country_name_by_addr_v6($gi, $_POST['ip']);
		else echo geoip_country_name_by_addr($gi, $_POST['ip']);
		break;
	}
	default: echo "Bad query type.";
}
	
geoip_close($gi);

?>
