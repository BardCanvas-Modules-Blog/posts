<?php
/**
 * Post trashes
 *
 * @package    BardCanvas
 * @subpackage categories
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_GET params
 * @param id_post
 */

use hng2_media\media_repository;
use hng2_modules\posts\posts_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

header("Content-Type: text/plain; charset=utf-8");
if( $account->state != "enabled" ) die($language->errors->access_denied);

if( empty($_GET["id_post"]) ) die($current_module->language->messages->missing->id);

$media_repository = new media_repository();
$posts_repository = new posts_repository();
$post             = $posts_repository->get($_GET["id_post"]);

if( is_null($post) ) die($current_module->language->messages->post_not_found);

$attached_media = $posts_repository->get_media_items($post->id_post);
if( ! empty($attached_media) )
{
    $item_ids  = array_keys($attached_media);
    
    $posts_repository->unset_all_media_items($post->id_post);
    //$media_repository->delete_multiple_if_unused($item_ids);
}

$current_module->load_extensions("post_actions", "before_trashing");
$posts_repository->trash($post->id_post);
$posts_repository->bump_index_caches();

$featured_posts_tag = $settings->get("modules:posts.featured_posts_tag");
if( is_array($post->tags_list) )
    if( in_array($featured_posts_tag, $post->tags_list) )
        $mem_cache->delete("modules:posts.featured_posts");

if( ! empty($post->main_category_slug) )
    if( stristr($settings->get("modules:posts.slider_categories"), $post->main_category_slug) !== false )
        $mem_cache->delete("modules:posts.slider_posts");

echo "OK";
