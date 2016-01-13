<?php
/*  this whole script is to stop people from accessing the source from external IPs.
**  The list of embaroged IPs is in restricted.php
**  The restircted.php file is created by a bash script and the IPS are in the fomrat
**  of array(low,hi) in an array.  The IP format is that of a decimal to speed up
**  comparisons instead of a string
**
**  This code simply checks if the IP address is clear, if it is, it asks you to
**  Certify that you are not from an embargoed nation and then sets a cookie for 
**  20 minutes.  I figured that you can download a TGZ in 20 minutes.
*/


/********************************************
**  CONFIGURATION
********************************************/
$g_contact_person="Brian Degnan";
$g_contact_email="degs"."@"."ece".".gatech.edu";

//software information
$g_softwarename="Simontool";
$g_softwaredescription="The ".$g_softwarename. " program is designed to assist in the implementation of the Simon Cipher.  This software implements all key and block sizes of the Simon Cipher.";

//embargo countries
$g_embargo_text="North Korea (DPRK), Iran, Syria, Sudan or Cuba";

//restricted file location
$g_restricted_directory="../private"; 

//cookies
$g_cookie_name="simontool";
$g_cookie_exipire=1200;  //20 minute expire
//$g_cookie_exipire=15;

$g_debug_restrict_me=0;  //if this is not 0, you'll ban yourself.


/********************************************
**  CODE BODY
********************************************/
$l_allclear=0;  //this is used in conjunction with the cookie for confirmation

if(!@include("restricted.php"))
{
   print "Error in configuration.";
   exit(1);
}
//when you POST, set the cookie because they hit confirm

//set the cookie to show that you passed the IP address test
if(isset($_POST['embargoconfirm'])) 
{
   setcookie($g_cookie_name, "embargo IP passed", time() + $g_cookie_exipire, "/");
   $l_allclear=1;
}

if((!isset($_COOKIE[$g_cookie_name]))&&($l_allclear==0)) {
	 if (!empty($_SERVER['HTTP_CLIENT_IP'])) {
		 $l_currentip = $_SERVER['HTTP_CLIENT_IP'];
	 } elseif (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
		 $l_currentip = $_SERVER['HTTP_X_FORWARDED_FOR'];
	 } else {
		 $l_currentip = $_SERVER['REMOTE_ADDR'];
	 }
	 $l_ip=ip2long($l_currentip);  //the iterations expect the IP address as a number.

	 //debugging and testing code to ban yourself.
	 if($g_debug_restrict_me!=0)
	 {
	   $l_ipbanlow=$l_ip-1;  //low value
	   $l_ipbanhigh=$l_ip+1; //high values
	   $a_restrictedip[]=array($l_ipbanlow,$l_ipbanhigh);  //add to restricted range
	 }

	 $g_IPsafe=1;   //this if the IP address is NOT in the banned list.

	 //look through ever IP range and determine if the IP lies in that range.
	 for ($l_counter = 0, $num_ips = count($a_restrictedip); $l_counter < $num_ips; $l_counter++) {
		//see if the ip address lies in a range.
		$ip_low = $a_restrictedip[$l_counter][0];
		$ip_hi  = $a_restrictedip[$l_counter][1];
   
		if(($l_ip>=$ip_low)&&($l_ip<=$ip_hi))
		{
		   $g_IPsafe=0;
		   break;
		}
	 }

	 //The message for embargoed IPs
	 if($g_IPsafe!=1)
	 {
		 echo("<br />You are unable to access this page because of embargo restrictions.<br />");
		 echo("If you believe that you are not in an embargoed State, please contact:<br />");
		 echo($g_contact_person."<br />");
		 echo($g_contact_email."<br />");
		 exit(0);
	 }else
	 {  

	 ?>
	 <html>
	 <head>
	 <title>Export Control</title>
	 </head>
	 <body style='width:600px;'>
	 <br /><b><?php echo($g_softwarename); ?></b>
	 <br /><br />
	 <?php echo($g_softwaredescription); ?>
	 <br /><br />
	 The <?php echo($g_softwarename); ?> software is software that contains encryption, and the source code is freely available.  However, this software is still controlled by <a href="http://www.bis.doc.gov/index.php/forms-documents/doc_view/838-772">EAR 772.1</a>.  This webpage exists for compliance with <a href="https://www.bis.doc.gov/index.php/forms-documents/doc_view/320-76-fr-1059">15 CFR 732</a>.<br />
	 <br />
	 <form action="<?php $_SERVER["SCRIPT_NAME"];?>" method="post"> 
    Do you confirm that you are not a citizen of an embargoed country (<?php echo($g_embargo_text)?>), or intend to export the software to an embargoed country?
   <!-- <input type="checkbox" name="formEmbargo" value="Yes" /> -->
    <input type="submit" name="embargoconfirm" value="Confirm" />
    </form>
    <br />
	 </body>
	 </html>
	 <?php
	 }
}else
{
  //at this point, we serve the page to give the files or give the file.
  //of course, if we send anything accidentally before this header, it won't work,
  //but the code is simple enough that I'm not worried about it.
if(isset($_GET['file']))
{
	//readfile($g_restricted_directory.'/'.$_GET['file']);
	header("Pragma: public", true);
	header("Expires: 0"); // set expiration time
	header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
	header("Content-Type: application/force-download");
	header("Content-Type: application/octet-stream");
	header("Content-Type: application/download");
	header("Content-Disposition: attachment; filename=".$_GET['file']);
	header("Content-Transfer-Encoding: binary");
	header("Content-Length: ".filesize($g_restricted_directory.'/'.$_GET['file']));
	die(file_get_contents($g_restricted_directory.'/'.$_GET['file']));
}else
{
  //select which file to serve up.
 ?>
	 <html>
	 <head>
	 <title>Export Control</title>
	 </head>
	 <body style='width:600px;'>
	 <br /><b><?php echo($g_softwarename); ?></b>
	 <br /><br />
	 <?php echo($g_softwaredescription); ?>
	 <br /><br />
	 The <?php echo($g_softwarename); ?> software is software that contains encryption, and the source code is freely available.  However, this software is still controlled by <a href="http://www.bis.doc.gov/index.php/forms-documents/doc_view/838-772">EAR 772.1</a>.  This webpage exists for compliance with <a href="https://www.bis.doc.gov/index.php/forms-documents/doc_view/320-76-fr-1059">15 CFR 732</a>.<br />
	 <br />
<?php
if ($handle = opendir($g_restricted_directory)) {
   while (false !== ($file = readdir($handle)))
      {
        if ($file != "." && $file != "..")
	  {
          	echo '<a href="'.$_SERVER["SCRIPT_NAME"]."?file=".$file.'">'.$file.'</a><br />';
          }
       }
  closedir($handle);
  }       
?>
    <br />
	 </body>
	 </html>
<?php
}
}



