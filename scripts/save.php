<?php
/**
 * Post saver
 *
 * @package    BardCanvas
 * @subpackage categories
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

use hng2_modules\posts\post_record;
use hng2_modules\posts\posts_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

header("Content-Type: text/plain; charset=utf-8");
if( ! $account->_exists ) die($language->errors->page_requires_login);

if( empty($_POST["title"]) )         die($current_module->language->messages->missing->title);
if( empty($_POST["content"]) )       die($current_module->language->messages->missing->content);
if( empty($_POST["main_category"]) ) die($current_module->language->messages->missing->main_category);

$repository = new posts_repository();

$old_post = empty($_POST["id_post"]) ? null : $repository->get($_POST["id_post"]);

$post       = new post_record();
$post->set_from_post();

if( empty($post->id_post) )
{
    $post->set_new_id();
    $post->id_author         = $account->id_account;
    $post->creation_date     = date("Y-m-d H:i:s");
    $post->creation_ip       = get_remote_address();
    $post->creation_host     = gethostbyaddr($post->creation_ip);
    $post->creation_location = forge_geoip_location($post->creation_ip);
    $post->last_update       = date("Y-m-d H:i:s");
    
    if( $post->status == "published" )
        $post->publishing_date   = date("Y-m-d H:i:s");
}

$excerpt_length = (int) $settings->get("modules:posts.excerpt_length");
if( empty($post->excerpt) ) $post->excerpt = make_excerpt_of(
    $post->content,
    empty($excerpt_length) ? 255 : $excerpt_length
);

if( empty($post->slug) ) $post->slug = sanitize_file_name($post->title);
$existing_slugs = $repository->get_record_count(array("slug like '{$post->slug}%'"));
if( $existing_slugs > 0 ) $post->slug .= "_" . $existing_slugs;

if( $post->main_category != $old_post->main_category )
    $repository->unset_category($old_post->main_category, $post->id_post);
$repository->set_category($_POST["main_category"], $post->id_post);

if( $item->status == "published" && $old_item->status != $item->status )
    $item->publishing_date = date("Y-m-d H:i:s");

$repository->save($post);

echo "OK";
