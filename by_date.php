<?php
/**
 * Frontend posts archive
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var template $template
 *             
 * $_GET params:
 * @param date
 */

use hng2_base\template;

include "../config.php";
include "../includes/bootstrap.inc";

if( empty($_GET["date"]) ) throw_fake_404();
if( strlen($_GET["date"]) > 10 ) throw_fake_404();

$evaling_date = str_replace("/", "-", $_GET["date"]);

if( strlen($evaling_date) == 10 ) # Day
{
    $parts = explode("-", $evaling_date);
    if( ! is_numeric($parts[0]) ) throw_fake_404();
    if( ! is_numeric($parts[1]) ) throw_fake_404();
    if( ! is_numeric($parts[2]) ) throw_fake_404();
    
    if( $parts[0] > date("Y") ) throw_fake_404();
    if( $parts[1] > 12 )        throw_fake_404();
    if( $parts[2] > 31 )        throw_fake_404();
    
    if( ! checkdate($parts[1], $parts[2], $parts[0]) )
        throw_fake_404();
    
    $showing_date = utf8_encode(strftime("%B %e, %Y", strtotime($evaling_date)));
    $template->set("showing_date", $showing_date);
    $start_date = $evaling_date . " 00:00:00";
    $end_date   = $evaling_date . " 23:59:59";
}
elseif( strlen($evaling_date) == 7 ) # Month
{
    $parts = explode("-", $evaling_date);
    if( ! is_numeric($parts[0]) ) throw_fake_404();
    if( ! is_numeric($parts[1]) ) throw_fake_404();
    
    if( $parts[0] > date("Y") ) throw_fake_404();
    if( $parts[1] > 12 )        throw_fake_404();
    
    $parts[2] = 1;
    if( ! checkdate($parts[1], $parts[2], $parts[0]) )
        throw_fake_404();
    
    $showing_date  = utf8_encode(strftime("%B %Y", strtotime("{$evaling_date}-01")));
    $start_date    = date("Y-m-d 00:00:00", strtotime("{$evaling_date}-01"));
    $end_timestamp = strtotime("{$start_date} + 1 month");
    $end_date      = date("Y-m-d 23:59:59", $end_timestamp - 86400);
}
else # Year
{
    if( ! is_numeric($evaling_date) ) throw_fake_404();
    if( $evaling_date > date("Y") ) throw_fake_404();
    
    if( ! checkdate(1, 1, $evaling_date) )
        throw_fake_404();
    
    $showing_date = $evaling_date;
    $start_date   = "{$evaling_date}-01-01 00:00:00";
    $end_date     = "{$evaling_date}-12-31 23:59:59";
}

$template->set("page_tag", "post_archive");
$template->set("showing_archive", true);
$template->set("raw_get_date", $_GET["date"]);
$template->page_contents_include = "by_date.inc";
$template->set_page_title(replace_escaped_vars(
    $current_module->language->pages->by_date->title, '{$date}', $template->get("showing_date")
));
$template->append("additional_body_attributes", " data-listing-type='archive'");
include "{$template->abspath}/main.php";
