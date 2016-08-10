<?php
/**
 * Frontend index of posts by hashtag
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var template $template
 * 
 * $_GET params:
 * @param tag
 */

include "../config.php";
include "../includes/bootstrap.inc";

use hng2_base\template;

$tag = trim(stripslashes($_GET["tag"]));
if( empty($tag) ) throw_fake_404();

$template->set("page_tag",        "post_tag_index");
$template->set("showing_archive", true);
$template->set("current_tag",     $tag);

$template->page_contents_include = "by_tag.inc";
$template->set_page_title(replace_escaped_vars(
    $current_module->language->pages->by_tag->title, '{$tag}', $tag
));
$template->append("additional_body_attributes", " data-listing-type='archive'");
include "{$template->abspath}/main.php";
