<?php
/**
 * Archives tree for left sidebar
 * Called as include() from within a function
 *
 * @package    HNG2
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Trailing vars:
 * @var module[]          $modules
 * @var module            $this_module    widgets_manager
 * @var template          $template       The caller
 * @var \SimpleXMLElement $widget
 * @var string            $seed
 * @var string            $data_key
 *
 * @return string
 */

use hng2_base\module;
use hng2_base\template;
use hng2_modules\posts\posts_repository;

global $config, $language, $account, $mem_cache, $settings;

$items = array();

/** @var \SimpleXMLElement $_lang */
$_lang = $this_module->language->widgets->left_sidebar_archives_tree;

$ttl = $settings->get("modules:posts.archives_tree_widget_cache_ttl"); if( $ttl == "" ) $ttl = 3;
$ttl = $ttl * 60 * 60;
$res = null;

$cache_key = "posts.archives_tree_widget";
if($ttl > 0) $res = $mem_cache->get($cache_key);

if( is_null($res) )
{
    $repository = new posts_repository();
    $res = $repository->get_archive_dates();
    if($ttl > 0) $mem_cache->set($cache_key, $res, 0, $ttl);
}

if( empty($res) ) return "";

foreach($res as $year => $months)
{
    $year_class  = date("Y") == $year ? "expanded" : "collapsed";
    
    $items[] = "
        <span class='item pseudo_link year $year_class' data-year='$year'>
            <span class='toggle right fa fa-fw fa-chevron-right'></span>
            <span class='toggle down  fa fa-fw fa-chevron-down'></span>
            $year
        </span>
    ";
    
    foreach($months as $month => $count)
    {
        $month         = sprintf("%02.0f", $month);
        $display_month = strftime("%B", strtotime("$year/$month/01"));
        $count         = empty($count) ? "" : "($count)";
    
        $selected = $template->get("page_tag") == "post_archive" 
                    && substr($template->get("raw_get_date"), 0, 7) == "$year/$month" 
                    ? "selected" : "";
        
        $items[] = "
            <a class='item month $selected $year_class' data-year='$year' href='{$config->full_root_path}/date/$year/$month'>
                <span class='fa fa-fw'></span>
                <span class='fa fa-caret-right'></span>
                $display_month $count
            </a>
        ";
    }
}

return implode("\n", $items);
