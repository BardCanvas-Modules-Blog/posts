<?php
/**
 * Posts toolbox
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var account           $account
 * @var settings          $settings
 * @var \SimpleXMLElement $language
 * 
 * $_GET params:
 * @param string "action"     change_status|untrash_for_review|empty_trash
 * @param string "new_status" trashed|published|reviewing|hidden|draft
 * @param string "id_post"
 */

use hng2_base\account;
use hng2_base\config;
use hng2_base\settings;
use hng2_media\media_repository;
use hng2_modules\posts\posts_repository;

header("Content-Type: text/plain; charset=utf-8");
include "../../config.php";
include "../../includes/bootstrap.inc";

if( ! $account->_exists ) die($language->errors->page_requires_login);
if( $account->level < (int) $settings->get("modules:posts.required_level_to_post") )
    die($language->errors->access_denied);

if( ! in_array($_GET["action"], array("change_status", "untrash_for_review", "empty_trash")) )
    die($current_module->language->messages->toolbox->invalid_action);

if( $_GET["action"] != "empty_trash" && empty($_GET["id_post"]) )
    die($current_module->language->messages->missing->id);

$repository       = new posts_repository();
$media_repository = new media_repository();

if($_GET["action"] == "empty_trash")
{
    if( ! $account->_is_admin ) die($current_module->language->messages->toolbox->action_not_allowed);
    $repository->empty_trash();
    die("OK");
}

$post = $repository->get($_GET["id_post"]);
if( is_null($post) ) die($current_module->language->messages->post_not_found);
$old_post = clone $post;

if($_GET["action"] == "change_status")
{
    if( ! in_array($_GET["new_status"], array("trashed", "published", "reviewing", "hidden", "draft")) )
        die($current_module->language->messages->toolbox->invalid_status);
    
    switch( $_GET["new_status"] )
    {
        case "published":
        {
            if($account->level < config::MODERATOR_USER_LEVEL)
                die($current_module->language->messages->toolbox->action_not_allowed);
            
            if( $post->status == "published" ) die("OK");
            
            $post->status = "published";
            include __DIR__ . "/save.inc";
            
            $current_module->load_extensions("post_actions", "after_publishing");
            die("OK");
            break;
        }
        case "reviewing":
        {
            if($account->level < config::MODERATOR_USER_LEVEL)
                die($current_module->language->messages->toolbox->action_not_allowed);
            
            if( $comment->status == "reviewing" ) die("OK");
            
            $res = $repository->change_status($post->id_post, "reviewing");
            
            $featured_posts_tag = $settings->get("modules:posts.featured_posts_tag");
            if( is_array($post->tags_list) )
                if( in_array($featured_posts_tag, $post->tags_list) )
                    $mem_cache->delete("modules:posts.featured_posts");
            
            if( ! empty($post->main_category_slug) )
                if( stristr($settings->get("modules:posts.slider_categories"), $post->main_category_slug) !== false )
                    $mem_cache->delete("modules:posts.slider_posts");
            
            $current_module->load_extensions("post_actions", "after_flagged_for_reviewing");
            if( empty($res) ) die("OK");
            
            die("OK");
            break;
        }
        case "trashed":
        {
            if( $post->status == "trashed" ) die("OK");
            
            if( ! $post->can_be_deleted() )
                die($current_module->language->messages->toolbox->action_not_allowed);
            
            $attached_media = $repository->get_media_items($_GET["id_post"]);
            if( ! empty($attached_media) )
            {
                $item_ids  = array_keys($attached_media);
                
                $repository->unset_all_media_items($_GET["id_post"]);
                $media_repository->delete_multiple_if_unused($item_ids);
            }
            
            $current_module->load_extensions("post_actions", "before_trashing");
            $repository->trash($_GET["id_post"]);
            
            $featured_posts_tag = $settings->get("modules:posts.featured_posts_tag");
            if( is_array($post->tags_list) )
                if( in_array($featured_posts_tag, $post->tags_list) )
                    $mem_cache->delete("modules:posts.featured_posts");
            
            if( ! empty($post->main_category_slug) )
                if( stristr($settings->get("modules:posts.slider_categories"), $post->main_category_slug) !== false )
                    $mem_cache->delete("modules:posts.slider_posts");
            
            die("OK");
            break;
        }
        case "hidden":
        {
            if($account->level < config::MODERATOR_USER_LEVEL)
                die($current_module->language->messages->toolbox->action_not_allowed);
            
            if( $post->status == "hidden" ) die("OK");
            
            if( ! $post->can_be_deleted() )
                die($current_module->language->messages->toolbox->action_not_allowed);
            
            $attached_media = $repository->get_media_items($_GET["id_post"]);
            if( ! empty($attached_media) )
            {
                $item_ids  = array_keys($attached_media);
                
                $repository->unset_all_media_items($_GET["id_post"]);
                $media_repository->delete_multiple_if_unused($item_ids);
            }
            
            $repository->hide($_GET["id_post"]);
            
            $featured_posts_tag = $settings->get("modules:posts.featured_posts_tag");
            if( is_array($post->tags_list) )
                if( in_array($featured_posts_tag, $post->tags_list) )
                    $mem_cache->delete("modules:posts.featured_posts");
            
            if( ! empty($post->main_category_slug) )
                if( stristr($settings->get("modules:posts.slider_categories"), $post->main_category_slug) !== false )
                    $mem_cache->delete("modules:posts.slider_posts");
            
            $current_module->load_extensions("post_actions", "after_hiding");
            
            die("OK");
            break;
        }
        case "draft":
        {
            if($account->level < config::MODERATOR_USER_LEVEL)
                die($current_module->language->messages->toolbox->action_not_allowed);
            
            if( $post->status == "draft" ) die("OK");
            
            $post->status = "draft";
            include __DIR__ . "/save.inc";
            
            $current_module->load_extensions("post_actions", "after_setting_as_draft");
            die("OK");
            break;
        }
    }
}

if($_GET["action"] == "untrash_for_review")
{
    if($account->level < config::MODERATOR_USER_LEVEL)
        die($current_module->language->messages->toolbox->action_not_allowed);
    
    if( $post->status == "published" ) die("OK");
    if( $post->status == "reviewing" ) die("OK");
    
    $res = $repository->change_status($post->id_post, "reviewing");
    $current_module->load_extensions("post_actions", "after_untrashing_for_review");
    
    die("OK");
}

die($language->errors->invalid_call);
