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
 * @var toolbox           $toolbox
 * 
 * @return string
 */

use hng2_base\module;
use hng2_base\template;
use hng2_modules\posts\posts_repository;
use hng2_modules\widgets_manager\toolbox;

global $config, $language, $account, $mem_cache, $settings;

if( empty($config->globals["widgets_manager:toolbox"]) )
    $config->globals["widgets_manager:toolbox"] = new toolbox();
$toolbox = $config->globals["widgets_manager:toolbox"];
$placed_widget = $toolbox->get_widget_from_data_key($data_key);
if( is_null($placed_widget) ) return "";
$data = $toolbox->load_widget_data($placed_widget);

$items = array();

$version   = $data["cache_version"]; if( empty($version) ) $version = 1;
$cache_ttl = $data["cache_ttl"] * 3600;
$cache_key = "modules:posts.archives_tree_widget-v{$version}";

$res = $mem_cache->get($cache_key);
if( empty($res) )
{
    $repository = new posts_repository();
    $res = $repository->get_archive_dates();
    $mem_cache->set($cache_key, $res, 0, $cache_ttl);
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
        $display_month = utf8_encode(strftime("%B", strtotime("$year/$month/01")));
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
