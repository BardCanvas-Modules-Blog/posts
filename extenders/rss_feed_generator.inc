<?php
/**
 * RSS feed generator extender
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Globals:
 * @var module   $current_module self
 * @var module   $this_module    self
 * 
 * Trailing vars (significant):
 * @var channel  $channel
 * @var template $template
 * @var string   $handle         Handle coming via GET
 * @var string   $handle_pattern The pattern as specified in the module_info.xml declaration
 * @var array    $handle_matches Matches of the handle in the pattern
 */

use hng2_base\module;
use hng2_base\account_record;
use hng2_base\accounts_repository;
use hng2_media\media_repository;
use hng2_base\template;
use hng2_modules\categories\categories_repository;
use hng2_modules\categories\category_record;
use hng2_modules\posts\posts_repository;
use hng2_rss\channel;
use hng2_rss\enclosure;
use hng2_rss\item;

if( preg_match('#^(author|user)/#i', $handle) )
    $content_key = "posts_by_author";
elseif( preg_match('#^category/#i', $handle) )
    $content_key = "posts_by_category";
else
    $content_key = "all_posts";

$channel->title       = $this_module->language->rss_feed_generator->{$content_key}->title;
$channel->description = $this_module->language->rss_feed_generator->{$content_key}->description;
$channel->link        = "{$config->full_root_url}/" . ($handle == "_none_" ? "" : "{$handle}/") . "feed";

$posts_repository      = new posts_repository();
$accounts_repository   = new accounts_repository();
$categories_repository = new categories_repository();

$author   = new account_record();
$category = new category_record();
$item_key = end(explode("/", $handle));

if( $content_key == "posts_by_author" )
    $author = $accounts_repository->get($item_key);
elseif( $content_key == "posts_by_category" )
    $category = $categories_repository->get($item_key);

$channel->title = replace_escaped_vars(
    $channel->title,
    array('{$website_name}', '{$author}', '{$category}'),
    array($settings->get("engine.website_name"), $author->display_name, $category->title)
);

$channel->description = replace_escaped_vars(
    $channel->description,
    array('{$website_name}', '{$author}', '{$category}'),
    array($settings->get("engine.website_name"), $author->display_name, $category->title)
);

if( $content_key == "posts_by_author"   && is_null($author)   ) return;
if( $content_key == "posts_by_category" && is_null($category) ) return;

if( $content_key == "posts_by_author" )
{
    $res     = $posts_repository->get_for_author($author->id_account);
    $records = $res->posts;
}
elseif( $content_key == "posts_by_category" )
{
    $res     = $posts_repository->get_for_category($category->id_category);
    $records = $res->posts;
}
else
{
    $records = $posts_repository->lookup(array(), 0, 0, "");
}

if( empty($records) ) return;

$media_repository = new media_repository();

foreach($records as $row)
{
    $item              = new item();
    $item->title       = html_entity_decode($row->title, null, "UTF-8");
    $item->link        = "{$config->full_root_url}/{$row->id_post}";
    $item->description = html_entity_decode($row->excerpt, null, "UTF-8");
    $item->guid        = "{$config->full_root_url}/{$row->id_post}";
    
    if( ! empty($row->id_featured_image) )
    {
        $media_item = $media_repository->get($row->id_featured_image);
        
        if( ! is_null($media_item) )
        {
            $item->enclosure = new enclosure();
            $item->enclosure->length = $media_item->size;
            $item->enclosure->type   = $media_item->mime_type;
            $item->enclosure->url    = $media_item->get_item_url(true);
        }
    }
    
    $channel->items[] = $item;
}
