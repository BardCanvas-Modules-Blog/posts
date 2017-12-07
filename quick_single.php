<?php
/**
 * Quick post form
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var template $template
 * @var account  $account
 * 
 * Incoming GET arguments:
 * @param bool "as_popup"
 * @param bool "no_attachments"
 */

use hng2_base\account;
use hng2_base\template;

include "../config.php";
include "../includes/bootstrap.inc";
if( $account->state != "enabled" ) throw_fake_401();
if( $account->level < (int) $settings->get("modules:posts.required_level_to_post") ) throw_fake_401();

$_GET["trigger_quick_post_form"] = "true";

$template->set_page_title($current_module->language->form->add_title);
if($_GET["as_popup"] == "true")
{
    $template->page_contents_include = "quick_post_form.inc";
    include "{$template->abspath}/popup.php";
}
else
{
    $template->page_contents_include = "contents/quick_post_form.inc";
    include "{$template->abspath}/admin.php";
}
