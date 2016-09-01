<?php
/**
 * Document handler for posts - By id, slug, etc.
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var template $template
 * 
 * $_GET params:
 * @param handle
 */

use hng2_base\template;
use hng2_modules\posts\posts_repository;

if( empty($_GET["handle"]) ) return;

$handle = trim(stripslashes($_GET["handle"]));

$repository = new posts_repository();
$post = $repository->get_by_id_or_slug($handle);

if( is_null($post) ) return;

$template->set_page_title($post->title);
$template->set("is_single_post", true);
$template->set("is_post", true);
$template->set("page_tag", "single_post");

$template->set("current_post",           $post);
$template->set("current_category_slug",  $post->main_category_slug);
$template->set("current_category_title", $post->main_category_title);
$template->set("current_category_id",    $post->main_category);

# @hack: The definition below is a hack that shouldn't be repeated unless strictly neccesary :P
$template->page_contents_include = "../../templates/{$template->name}/segments/single_post_renderer.inc";
include "{$template->abspath}/main.php";
exit;
