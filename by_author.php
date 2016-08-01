<?php
/**
 * Frontend index of posts by author
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var template $template
 * 
 * $_GET params:
 * @param slug
 */

use hng2_base\account;
use hng2_base\template;

include "../config.php";
include "../includes/bootstrap.inc";

if( empty($_GET["slug"]) ) throw_fake_404();

$author = new account($_GET["slug"]);
if( ! $author->_exists ) throw_fake_404();

$template->set("page_tag", "post_author_index");
$template->set("showing_archive", true);
$template->set("show_user_profile_heading", true);
$template->set("user_profile_account", $author);
$template->set("current_user_profile_tab", "posts");
$template->page_contents_include = "by_author.inc";
$template->set_page_title(replace_escaped_vars(
    $current_module->language->pages->by_author->title, '{$author}', $author->display_name
));
$template->append("additional_body_attributes", " data-listing-type='archive'");
include "{$template->abspath}/main.php";
