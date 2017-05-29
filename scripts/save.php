<?php
/**
 * Post saver
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var account           $account
 * @var settings          $settings
 * @var \SimpleXMLElement $language
 * @var module            $current_module
 *
 * $_POST extras for quick post form:
 * @param string $_POST["ok_with_url"] To return OK:post_url insetad of just OK
 *                                     (Used on the quick post form)
 * @param array $_FILES[attachments][image|video][]
 */

use hng2_base\account;
use hng2_base\config;
use hng2_base\module;
use hng2_media\media_repository;
use hng2_base\settings;
use hng2_modules\categories\categories_repository;
use hng2_modules\posts\post_record;
use hng2_modules\posts\posts_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

header("Content-Type: text/plain; charset=utf-8");
if( ! $account->_exists ) die($language->errors->page_requires_login);
if( $account->level < (int) $settings->get("modules:posts.required_level_to_post") )
    die($language->errors->access_denied);

if( empty($_POST["title"]) )
    die($current_module->language->messages->missing->title);

if( preg_match('/http|https|www\./i', stripslashes($_POST["title"])) )
    die($current_module->language->messages->no_urls_in_title);

if( empty($_POST["main_category"]) )
    die($current_module->language->messages->missing->main_category);

if( empty($_FILES["attachments"]) && empty($_POST["content"]) )
    die($current_module->language->messages->missing->content);

$repository       = new posts_repository();
$media_repository = new media_repository();

$categories_repository = new categories_repository();
$category = $categories_repository->get($_POST["main_category"]);
if( $category->visibility == "level_based" && $account->level < $category->min_level )
    die($current_module->language->messages->invalid_category_selected);

$old_post = empty($_POST["id_post"]) ? null : clone $repository->get($_POST["id_post"]);
$post     = empty($_POST["id_post"]) ? new post_record() : $repository->get($_POST["id_post"]);
$post->set_from_post();

if( $_POST["is_quick_post"] == "true" ) $post->allow_comments = 1;
if( $account->level < config::MODERATOR_USER_LEVEL ) $post->allow_comments = 1;

include __DIR__ . "/save.inc";

if( $_POST["is_quick_post"] && $post->status == "draft" )
    send_notification($account->id_account, "success", $current_module->language->messages->draft_saved);

//if( is_array($media_deletions) && ! empty($media_deletions) )
//    $media_repository->delete_multiple_if_unused($media_deletions);

if( $_POST["ok_with_url"] == "true" )
    echo "OK:{$post->get_permalink()}";
elseif( $post->status == 'draft' )
    echo "OK:{$post->id_post}";
else
    echo "OK";
