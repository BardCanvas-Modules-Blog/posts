<?php
/**
 * Remove a single tag from post contents
 *
 * @package    BardCanvas
 * @subpackage categories
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_GET params
 * @param id_post
 * @param tag
 */

use hng2_modules\posts\posts_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

header("Content-Type: text/plain; charset=utf-8");
if( ! $account->_exists ) die($language->errors->page_requires_login);

if( empty($_GET["id_post"]) ) die($current_module->language->messages->missing->id);
if( empty($_GET["tag"]) ) die($current_module->language->messages->missing->tag);

$repository = new posts_repository();
$post       = $repository->get($_GET["id_post"]);

if( is_null($post) ) die($current_module->language->messages->post_not_found);

$tag           = trim($_GET["tag"]);
$post->title   = preg_replace("/#$tag\\b/i", "", $post->title);
$post->content = preg_replace("/#$tag\\b/i", "", $post->content);
$post->excerpt = preg_replace("/#$tag\\b/i", "", $post->excerpt);

$repository->save($post);
$repository->delete_tag($post->id_post, $tag);

send_notification($account->id_account, "success", replace_escaped_vars(
    $current_module->language->messages->tag_removed,
    array('{$tag}', '{$post_title}'),
    array($tag, $post->title)
));

echo "OK";
