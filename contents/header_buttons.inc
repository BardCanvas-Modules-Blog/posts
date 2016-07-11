<?php
/**
 * Posts header buttons
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

if ( ! $account->_exists) return;

$added_class = $current_module->name == $this_module->name ? "current" : "";
$wasuuup     = md5(mt_rand(1, 65535));
$href        = $_SERVER["REQUEST_URI"];

$template->add_menu_item(
    $this_module->language->browser->menu_caption,
    "
    <a class='main_menu_item {$added_class}' href='{$config->full_root_path}/posts/'>
        <span class='fa fa-newspaper-o'></span>
        {$this_module->language->index->menu_caption}
    </a>
    "
);
