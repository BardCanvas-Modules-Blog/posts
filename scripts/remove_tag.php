<?php
/**
 * Remove a single tag from post contents
 *
 * @package    BardCanvas
 * @subpackage categories
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_GET params
 * @param int    id_post
 * @param string tag
 */

use hng2_modules\posts\posts_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

header("Content-Type: text/plain; charset=utf-8");
if( $account->state != "enabled" ) die($language->errors->access_denied);
if( $account->level < (int) $settings->get("modules:posts.required_level_to_post") )
    die($language->errors->access_denied);

$_GET["id_post"] = $_GET["id_post"] + 0;

if( empty($_GET["id_post"]) ) die($current_module->language->messages->missing->id);
if( empty($_GET["tag"]) ) die($current_module->language->messages->missing->tag);
if( has_injected_scripts($tag) ) die($current_module->language->messages->invalid_tag);

$repository = new posts_repository();
$post       = $repository->get($_GET["id_post"]);

if( is_null($post) ) die($current_module->language->messages->post_not_found);

$tag           = trim($_GET["tag"]);
$post->title   = preg_replace("/#$tag\\b/i", "", $post->title);
$post->content = preg_replace("/#$tag\\b/i", "", $post->content);
$post->excerpt = preg_replace("/#$tag\\b/i", "", $post->excerpt);

$repository->save($post);
$repository->delete_tag($post->id_post, $tag);
$repository->bump_index_caches();

send_notification($account->id_account, "success", replace_escaped_vars(
    $current_module->language->messages->tag_removed,
    array('{$tag}', '{$post_title}'),
    array($tag, $post->title)
));

if( $tag == $settings->get("modules:posts.featured_posts_tag") )
    $mem_cache->delete("modules:posts.featured_posts");

echo "OK";
