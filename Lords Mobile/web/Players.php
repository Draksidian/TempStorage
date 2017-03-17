<?php
include("db_connection.php");

if(!isset($FilterK))
	$FilterK = 67;

if (substr_count($_SERVER["HTTP_ACCEPT_ENCODING"], "gzip"))
	ob_start("ob_gzhandler"); 
else 
	ob_start();

$SimpleView = 1;
if(isset($FN) || isset($FG) )
	$SimpleView = 0;
else
	echo "To minimize page size only player coordinates are shown. If you wish to get more info, click on player name<br>";
?>
<link href="css/table.css" rel="stylesheet">
Hidden players are not shown!<br>
Selected kingdom is <?php echo $FilterK;?><br>
<table>
  <thead style="background-color: #60a917">
	<tr>
		<td>x</td>
		<td>y</td>
		<td>Name</td>
		<td>Guild</td>
		<?php if( $SimpleView == 0 )
		{
			?>
		<td>Might</td>
		<td>Kills</td>
		<td>Guild rank</td>
		<td>VIP Level</td>
		<td>Castle Level</td>
			<?php
		}
		?>
		<td>Last Updated</td>
<!--		<td>Last Burned at</td>
		<td>Player Level</td>
		<td>Last seen with prisoners</td>
		<td>Innactive</td>
		<td>Last Burned at might</td>
		<td>Aprox troops available</td>
		<td>Nodes gathering from</td>
		<td>Castle lvl</td>
		<td>Bounty</td>
		<td>Distance to hive</td>
		<td>Active at X hours</td>
		<td>Active Y hours a day</td>
		<td>First seen ever(age)</td> -->
	</tr>
  </thead>
  <tbody class="TFtable">
	<?php
	// do not show hidden players
	$HiddenNames = "";
	$query1 = "select name from players_hidden where EndStamp > ".time();
//echo "$query1<br>";
	$result1 = mysql_query($query1,$dbi) or die("2017022001".$query1);
	while( list( $name ) = mysql_fetch_row( $result1 ) )
		$HiddenNames .= "####$name####";

	$HiddenGuilds = "";
	$Filter = "";
	$Order = " lastupdated desc ";
	$query1 = "select name from guilds_hidden where EndStamp > ".time();
//echo "$query1<br>";
	$result1 = mysql_query($query1,$dbi) or die("2017022001".$query1);
	while( list( $name ) = mysql_fetch_row( $result1 ) )
		$HiddenGuilds .= "####$name####";
		
	$query1 = "select x,y,name,guild,might,kills,lastupdated,innactive,HasPrisoners,VIP,GuildRank,PLevel,castlelevel from ";
	if(isset($FN))
		$query1 .= "players_archive ";
	else
		$query1 .= "players ";
	
//	if($FilterK)
//		$Filter .= " and k='".mysql_real_escape_string($FilterK)."' ";
	if(isset($FN))
	{
		//remove "guild" from player name
		$IsInGuildPos = strpos($FN,']');
		if($IsInGuildPos>0)
			$namename = substr($FN,$IsInGuildPos);
		else
			$namename = $FN;
		$Filter .= " and ( name like '%]".mysql_real_escape_string($namename)."' or name like '".mysql_real_escape_string($namename)."')";
	}
	if(isset($FG))
		$Filter .= " and guild like '".mysql_real_escape_string($FG)."' ";
	
	if($Filter)
		$query1 .= " where 1=1 $Filter ";
	if($Order)
		$query1 .= " order by $Order ";
	
	if( isset($FN) )
	{
		$query1 = str_replace(" order by $Order ","", $query1);
		$q2 = str_replace("players_archive","players", $query1);
		$query1 = "($query1)union($q2) order by $Order";
	}
//echo $FN.":".$query1;
	
	$result1 = mysql_query($query1,$dbi) or die("2017022001".$query1);
	while( list( $x,$y,$name,$guild,$might,$kills,$lastupdated,$innactive,$HasPrisoners,$VIP,$GuildRank,$Plevel,$castlelevel ) = mysql_fetch_row( $result1 ))
	{
		$IsInGuildPos = strpos($name,']');
		if($IsInGuildPos>0)
			$namename = substr($name,$IsInGuildPos+1);
		else
			$namename = $name;
		
		if( strpos($HiddenNames,"#".$name."#") != 0 )
			continue;
		if( strpos($HiddenGuilds,"#".$guild."#") != 0 )
			continue;
		
		$LastUpdatedHumanFormat = gmdate("Y-m-d\TH:i:s\Z", $lastupdated);
		//$innactiveHumanFormat = gmdate("Y-m-d\TH:i:s\Z", $innactive);
		$PlayerArchiveLink = "?FN=".urlencode($namename);
		$GuildFilterLink = "?FG=".urlencode($guild);
		$HasPrisonersHumanFormat = gmdate("Y-m-d\TH:i:s\Z", $HasPrisoners);	
		$LastUpdatedAsDiff = GetTimeDiffShortFormat($lastupdated);
		$HasPrisonersAsDiff = GetTimeDiffShortFormat($HasPrisoners);
		if($HasPrisonersAsDiff=="48.4 y")
			$HasPrisonersAsDiff="";
		if($guild=="")
			$guild="&nbsp;";
/*			<td><?php echo $HasPrisonersAsDiff; ?></td>
			<td><?php echo $innactive; ?></td> 
			<td><?php echo $Plevel; ?></td>
			*/
?>
<tr>
<td><?php echo $x; ?></td>
<td><?php echo $y; ?></td>
<td><a href="<?php echo $PlayerArchiveLink; ?>"><?php echo $namename; ?></a></td>
<td><a href="<?php echo $GuildFilterLink; ?>"><?php echo $guild; ?></a></td>
<?php if( $SimpleView == 0 )
{
?>
<td><?php echo GetValShortFormat($might); ?></td>
<td><?php echo GetValShortFormat($kills); ?></td>
<td><?php echo $GuildRank; ?></td>
<td><?php echo $VIP; ?></td>
<td><?php echo $castlelevel; ?></td>
<?php
}
?>
<td><?php echo $LastUpdatedAsDiff; ?></td>
</tr>
<?php
}
?>	
  </tbody>
</table>
