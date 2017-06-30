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

use hng2_base\config;
use hng2_base\template;
use hng2_modules\posts\posts_repository;

if( empty($_GET["handle"]) ) return;

$handle = trim(stripslashes($_GET["handle"]));

$repository = new posts_repository();
$post = $repository->get_by_id_or_slug($handle);
if( is_null($post) ) return;

if( $account->level < config::MODERATOR_USER_LEVEL )
{
    if( $post->status == "hidden" ) throw_fake_404();
    if( $post->status == "trashed" ) throw_fake_404();
    if( $post->status == "draft" && $post->id_author != $account->id_account ) throw_fake_401();
    if( $post->status == "reviewing" && $post->id_author != $account->id_account ) throw_fake_401();
    if( $post->expiration_date != "0000-00-00 00:00:00"
        && date("Y-m-d H:i:s") > $post->expiration_date ) throw_fake_401();
    if( $post->visibility == "private" && $account->id_account != $post->id_author ) throw_fake_401();
}

if( ($post->visibility == "users" || $post->main_category_visibility == "users") && ! $account->_exists )
    throw_fake_401();

if( $post->visibility == "level_based" && $account->level < $post->author_level )
    throw_fake_401();

if( $post->main_category_visibility == "level_based" && $account->level < $post->main_category_min_level )
    throw_fake_401();

if( $post->status == "reviewing" ) $post->allow_comments = 0;
if( $post->status == "published" ) $repository->increment_views($post->id_post);

$template->set_page_title($post->title);
$template->page_meta_description   = $post->excerpt;
$template->page_meta_url           = $post->get_permalink(true);
$template->page_meta_image         = $config->full_root_url . $post->featured_image_thumbnail;
$template->page_extra_metas["og:type"] = "<meta property='og:type' content='article'>";

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
