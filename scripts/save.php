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

# if( $post->main_category != $old_post->main_category )
#     $repository->unset_category($old_post->main_category, $post->id_post);
# $repository->set_category($_POST["main_category"], $post->id_post);

if( $post->status == "published" && $old_post->status != $post->status )
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
            
            $post->expiration_date = date("Y-m-d H:i:s", strtotime($post->publishing_date) + ($hours * 3600));
            break;
        }
    }
}

$tags = extract_hash_tags($post->title . " " . $post->content);

if( ! empty($_FILES["attachments"]) )
{
    # Coming from quick post... contents may be empty and only images/media are coming...
    $uploads = array();
    
    foreach($_FILES["attachments"] as $field => $types)
        foreach($types as $type => $entries)
            foreach($entries as $index => $value)
                $uploads[$type][$index][$field] = $value;
    
    $media_repository = new media_repository();
    
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
                "description"    => "{$post->title}\n\n{$post->excerpt}",
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
                    style='width: auto; height: 90vh;' {$item->id_media}'></p>\n";
            else
                $post->content .= "\n<p><img src='{$item->get_thumbnail_url()}' 
                    style='width: auto; height: 90vh;' data-id-media='{$item->id_media}' 
                    data-media-type='video' data-href='{$item->get_item_embeddable_url(true)}'></p>\n";
        }
    }
}

if( ! empty($tags) ) $repository->set_tags($tags, $post->id_post);

$repository->save($post);

if( $_POST["ok_with_url"] == "true" )
    echo "OK:{$post->get_permalink()}";
else
    echo "OK";
