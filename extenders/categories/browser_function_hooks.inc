<?php
/**
 * Categories extender: browser function hooks
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * Imported globals:
 * @var array $per_column_function_hooks 2-dimensional: field keys, called function names
 */

use hng2_modules\categories\category_record;

global $per_column_function_hooks;

$per_column_function_hooks["actions"][] = "render_link_for_posts_category_index";
$per_column_function_hooks["important_details"][] = "render_posts_important_details_in_category_row";

/**
 * @param category_record $record
 */
function render_link_for_posts_category_index($record)
{
    global $modules;
    
    $this_module = $modules["posts"];
    
    echo "
        <a class='action alternate' target='_blank' href='/category/{$record->slug}'>
            <span class='fa fa-external-link'></span>
            {$this_module->language->extenders->categories_browser->show_category_posts}
        </a>
    ";
}

/**
 * @param category_record $record
 */
function render_posts_important_details_in_category_row($record)
{
    global $modules, $settings, $config;
    
    /** @var array $exclussions */
    $exclussions = $config->globals["modules:posts.home_excluded_categories_array"];
    if( ! is_array($exclussions) )
    {
        $raw_exclussions = $settings->get("modules:posts.home_excluded_categories");
        if( empty($raw_exclussions) ) return;
        
        $exclussions = array();
        foreach(explode("\n", $raw_exclussions) as $line)
            $exclussions[] = trim($line);
        
        $config->globals["modules:posts.home_excluded_categories_array"] = $exclussions;
    }
    
    $this_module = $modules["posts"];
    $slug        = $record->slug;
    
    $found = false;
    foreach($exclussions as $exclussion)
    {
        if( stristr($exclussion, $slug) !== false )
        {
            $found = true;
            
            break;
        }
    }
    if( ! $found ) return;
    
    echo "
        <span class='framed_content inlined state_highlight' style='margin: 5px 0 0 0;'>
            <span class='fa fa-ban'></span>
            {$this_module->language->extenders->categories_browser->excluded_from_main_index}
        </span>
    ";
}
