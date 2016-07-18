<?php
/**
 * Frontend index of posts by author
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_GET params:
 * @param slug
 */

use hng2_base\account;

include "../config.php";
include "../includes/bootstrap.inc";

if( empty($_GET["slug"]) ) throw_fake_404();

$author = new account($_GET["slug"]);
if( ! $author->_exists ) throw_fake_404();

$template->set("showing_archive", true);
$template->page_contents_include = "by_author.inc";
$template->set_page_title(replace_escaped_vars(
    $current_module->language->pages->by_author->title, '{$author}', $author->display_name
));
$template->append("additional_body_attributes", " data-listing-type='archive'");
include "{$template->abspath}/main.php";
