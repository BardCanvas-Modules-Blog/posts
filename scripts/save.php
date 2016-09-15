<?php
/**
 * Post saver
 *
 * @package    BardCanvas
 * @subpackage categories
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

if( empty($_POST["title"]) )
    die($current_module->language->messages->missing->title);

if( empty($_POST["main_category"]) )
    die($current_module->language->messages->missing->main_category);

if( empty($_FILES["attachments"]) && empty($_POST["content"]) )
    die($current_module->language->messages->missing->content);

$repository       = new posts_repository();
$media_repository = new media_repository();

$old_post = empty($_POST["id_post"]) ? null : $repository->get($_POST["id_post"]);
$post     = empty($_POST["id_post"]) ? new post_record() : $repository->get($_POST["id_post"]);
$post->set_from_post();

if( $_POST["is_quick_post"] == "true" ) $post->allow_comments = 1;
if( $account->level < config::MODERATOR_USER_LEVEL ) $post->allow_comments = 1;

$current_module->load_extensions("save_post", "initial_validations");

if( ! empty($post->id_post) )
{
    $time = (int) $settings->get("modules:posts.time_allowed_for_editing_after_publishing");
    if( ! $post->can_be_edited() )
    {
        if(empty($time)) die(unindent($current_module->language->messages->post_cannot_be_edited->without_timing));
        else             die(unindent($current_module->language->messages->post_cannot_be_edited->with_timing));
    }
}
else
{
    $config->globals["posts:submitted_post_is_new"] = true;
    $post->set_new_id();
    $post->id_author         = $account->id_account;
    $post->creation_date     = date("Y-m-d H:i:s");
    $post->creation_ip       = get_remote_address();
    $post->creation_host     = gethostbyaddr($post->creation_ip);
    $post->creation_location = forge_geoip_location($post->creation_ip);
    $post->last_update       = date("Y-m-d H:i:s");
}

$excerpt_length = (int) $settings->get("modules:posts.excerpt_length");
if( empty($post->excerpt) ) $post->excerpt = make_excerpt_of(
    $post->content,
    empty($excerpt_length) ? 250 : $excerpt_length
);

if( empty($post->slug) ) $post->slug = sanitize_file_name($post->title);
$existing_slugs = $repository->get_record_count(array("slug like '{$post->slug}%'"));
if( $existing_slugs > 0 ) $post->slug .= "_" . $existing_slugs;

$current_module->load_extensions("save_post", "after_record_forging");

# if( $post->main_category != $old_post->main_category )
#     $repository->unset_category($old_post->main_category, $post->id_post);
# $repository->set_category($_POST["main_category"], $post->id_post);

# Enforced expiration date by category
$set_expiration_date = "";
if( $post->status == "published" && (empty($post->publishing_date) || $post->publishing_date == "0000-00-00 00:00:00") )
{
    $post->publishing_date = date("Y-m-d H:i:s");
    
    $res = $settings->get("modules:posts.enforced_expiration_by_category");
    if( ! empty($res) )
    {
        $categories_repository = new categories_repository();
        $entries = explode("\n", $res);
        foreach($entries as $entry)
        {
            list($slug, $hours) = preg_split('/\s*-\s*/', $entry);
            $id = $categories_repository->get_id_by_slug($slug);
            if( $post->main_category != $id ) continue;
            
            $set_expiration_date = date("Y-m-d H:i:s", strtotime($post->publishing_date) + ($hours * 3600));
            break;
        }
    }
}

# This goes here since tags are shared with media items
$tags = extract_hash_tags($post->title . " " . $post->content);
$featured_posts_tag = $settings->get("modules:posts.featured_posts_tag");
if(
    $account->level < config::MODERATOR_USER_LEVEL
    && $settings->get("modules:posts.show_featured_posts_tag_everywhere") != "true"
    && ! empty($featured_posts_tag)
    && in_array($featured_posts_tag, $tags)
) {
    unset($tags[array_search($featured_posts_tag, $tags)]);
    $post->title   = str_replace("#$featured_posts_tag", $featured_posts_tag, $post->title);
    $post->content = str_replace("#$featured_posts_tag", $featured_posts_tag, $post->content);
    $post->excerpt = str_replace("#$featured_posts_tag", $featured_posts_tag, $post->excerpt);
}

if( ! empty($_FILES["attachments"]) )
{
    # Coming from quick post... contents may be empty and only images/media are coming...
    $uploads = array();
    
    foreach($_FILES["attachments"] as $field => $types)
        foreach($types as $type => $entries)
            foreach($entries as $index => $value)
                $uploads[$type][$index][$field] = $value;
    
    foreach($uploads as $type => $files)
    {
        /** @var  array $file [name, type, tmp_name, error, size] */
        foreach($files as $index => $file)
        {
            $file_title = "{$account->display_name} - {$file["name"]}";
            
            if( $media_repository->get_record_count(array("title" => $file_title)) )
                die( $file["name"] . "\n" . $modules["gallery"]->language->messages->item_exists );
        }
        
        /** @var  array $file [name, type, tmp_name, error, size] */
        foreach($files as $index => $file)
        {
            $file_title = "{$account->display_name} - {$file["name"]}";
            
            $item_data = array(
                "title"          => $file_title,
                "description"    => "{$post->title}\n\n{$post->excerpt}" .
                                    (empty($tags) ? "" : "\n\n#" . implode(" #", $tags)),
                "main_category"  => $post->main_category,
                "visibility"     => $post->visibility,
                "status"         => "published",
                "password"       => "",
                "allow_comments" => "1",
            );
            $res = $media_repository->receive_and_save($item_data, $file, true);
            
            if( is_string($res) ) die($res);
            
            $item = $res;
            
            if( $type == "image" )
                $post->content .= "\n<p><img src='{$item->get_item_url()}'
                    data-media-type='image' data-id-media='{$item->id_media}'></p>\n";
            else
                $post->content .= "\n<p><img src='{$item->get_thumbnail_url()}' 
                    data-media-type='video' data-id-media='{$item->id_media}' 
                    data-href='{$item->get_item_embeddable_url(true)}'></p>\n";
        }
    }
}

$media_items = array();
if( function_exists("extract_media_items") )
{
    $images = extract_media_items("image", $post->content);
    $videos = extract_media_items("video", $post->content);
    $media_items = array_merge($images, $videos);
}

$media_deletions = array();
if( $post->status == "published" )
{
    $repository->set_tags($tags, $post->id_post);
    $media_deletions = $repository->set_media_items($media_items, $post->id_post);
}
$repository->save($post);
if( $set_expiration_date ) $repository->set_expiration_date($post->id_post, $set_expiration_date);
$current_module->load_extensions("save_post", "after_saving");

if( $_POST["is_quick_post"] && $post->status == "draft" )
    send_notification($account->id_account, "success", $current_module->language->messages->draft_saved);

if( is_array($media_deletions) && ! empty($media_deletions) )
{
    
    $media_repository->delete_multiple_if_unused($media_deletions);
}

if( $_POST["ok_with_url"] == "true" )
    echo "OK:{$post->get_permalink()}";
elseif( $post->status == 'draft' )
    echo "OK:{$post->id_post}";
else
    echo "OK";
