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
if( $account->state != "enabled" ) die($language->errors->access_denied);
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

$custom_fields = array();
$custom_fields_editing_level = $settings->get("modules:posts.level_allowed_to_edit_custom_fields");
if( empty($custom_fields_editing_level) )
{
    $custom_fields_editing_level = config::MODERATOR_USER_LEVEL;
    $settings->set("modules:posts.level_allowed_to_edit_custom_fields", $custom_fields_editing_level);
}
if( $account->level >= $custom_fields_editing_level )
{
    if( ! empty($_POST["custom_field_names"]) )
    {
        foreach($_POST["custom_field_names"] as $index => $name)
        {
            $name  = trim(stripslashes($name));
            $value = trim(stripslashes($_POST["custom_field_values"][$index]));
            if( empty($name) ) continue;
            
            $custom_fields[$name] = $value;
            unset( $_POST["custom_field_names"][$index] );
            unset( $_POST["custom_field_values"][$index] );
        }
    }
}

if( $_POST["is_quick_post"] == "true" ) $post->allow_comments = 1;
if( $account->level < config::MODERATOR_USER_LEVEL ) $post->allow_comments = 1;

include __DIR__ . "/save.inc";

if( $_POST["is_quick_post"] && $post->status == "draft" )
    send_notification($account->id_account, "success", $current_module->language->messages->draft_saved);

if( $account->level >= $custom_fields_editing_level )
{
    $post->purge_metas();
    foreach($custom_fields as $name => $value)
        $post->set_meta($name, $value);
}

//if( is_array($media_deletions) && ! empty($media_deletions) )
//    $media_repository->delete_multiple_if_unused($media_deletions);

if( $post->status == "published" ) $repository->bump_index_caches();

if( $_POST["ok_with_url"] == "true" )
    echo "OK:{$post->get_permalink()}";
elseif( $post->status == 'draft' )
    echo "OK:{$post->id_post}";
else
    echo "OK";
