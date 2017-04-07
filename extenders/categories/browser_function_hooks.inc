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
