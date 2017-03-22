<?php
/**
 * Posts browser
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * @var template $template
 * @var account  $account
 */

use hng2_base\account;
use hng2_base\template;

include "../config.php";
include "../includes/bootstrap.inc";
if( ! $account->_exists ) throw_fake_404();
if( $account->level < (int) $settings->get("modules:posts.required_level_to_post") ) throw_fake_401();

$template->page_contents_include = "contents/index.inc";
$template->set_page_title($current_module->language->index->title);
include "{$template->abspath}/admin.php";
