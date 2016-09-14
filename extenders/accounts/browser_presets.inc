<?php
/**
 * Accounts extender - browser presets
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Globals:
 * @var module $current_module accounts
 * @var module $this_module    self
 * @var mixed  $order
 */

use hng2_base\module;

global $order;

list($sqlorder, $additional_select_columns) = $config->globals["accounts:nav/presettings_data"];

switch( $order )
{
    case "posts_count_asc":  $sqlorder = "_posts_count asc";  break;
    case "posts_count_desc": $sqlorder = "_posts_count desc"; break;
}

$additional_select_columns .= "
    , (select count(id_post) from posts where posts.id_author = account.id_account ) as _posts_count
";

$config->globals["accounts:nav/presettings_data"] = array( $sqlorder, $additional_select_columns);
