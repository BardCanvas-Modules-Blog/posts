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

include_once ROOTPATH . "/lib/phpQuery-onefile.php";

if( preg_match('#^(author|user)/#i', $handle) )
    $content_key = "posts_by_author";
elseif( preg_match('#^category/#i', $handle) )
    $content_key = "posts_by_category";
elseif( preg_match('#^tag/#i', $handle) )
    $content_key = "posts_by_tag";
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
$array    = explode("/", $handle);
$item_key = end($array);

if( ! empty($item_key) )
{
    try
    {
        check_sql_injection(array($item_key));
    }
    catch(\Exception $e)
    {
        return;
    }
}

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
if( $content_key == "posts_by_category" && $category->visibility !== "public" ) return;

$config->globals["build_find_params:inject_incoming_nav_params"] = true;
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
elseif( $content_key == "posts_by_tag" )
{
    $records = $posts_repository->get_for_feed_in_tag($item_key);
}
else
{
    $records = $posts_repository->get_for_feed();
}

if( empty($records) ) return;

$media_repository    = new media_repository();
$include_contents    = $settings->get("modules:posts.export_contents_in_feeds") == "true";
$permalinks_as_slugs = $settings->get("modules:posts.permalink_style") == "slug";

foreach($records as $row)
{
    $item              = new item();
    $item->title       = strip_tags(html_entity_decode($row->title, null, "UTF-8"));
    $item->link        = $permalinks_as_slugs
                       ? "{$config->full_root_url}/{$row->slug}"
                       : "{$config->full_root_url}/{$row->id_post}";
    $item->description = strip_tags(html_entity_decode($row->excerpt, null, "UTF-8"));
    $item->guid        = "{$config->full_root_url}/{$row->id_post}";
    
    if( $include_contents || $_GET["include_contents"] == "true" )
    {
        $item->description = externalize_urls($row->get_processed_content());
        $tmp = phpQuery::newDocumentHTML($item->description);
        $tmp->find("*[data-media-type]")->each(function($e) { pq($e)->removeAttr("data-media-type"); });
        $tmp->find("*[data-id-media]")->each(function($e) { pq($e)->removeAttr("data-id-media"); });
        $tmp->find("video")->each(function($e) { pq($e)->removeAttr("poster")->removeAttr("style")->removeAttr("controls"); });
        $tmp->find("*[style*='caret-color']")->each(function($e) { pq($e)->removeAttr("style"); });
        $tmp->find('.play_trigger ')->remove();
        $item->description = trim( $tmp->html() );
        $item->description = str_replace("<p></p>",       "", $item->description);
        $item->description = str_replace("<p>&nbsp;</p>", "", $item->description);
        $item->description = str_replace("<p> </p>",      "", $item->description);
    }
    
    if( ! empty($row->id_featured_image) )
    {
        $media_item = $media_repository->get($row->id_featured_image);
        
        if( ! is_null($media_item) )
        {
            $item->enclosure         = new enclosure();
            $item->enclosure->length = $media_item->size;
            $item->enclosure->type   = $media_item->mime_type;
            $item->enclosure->url    = $media_item->get_item_url(true);
        }
    }
    
    $item->author   = $row->author_display_name;
    $item->category = $row->main_category_title;
    $item->comments = $item->link;
    $item->pubDate  = gmdate('D, d M Y H:i:s T', strtotime($row->publishing_date) );
    
    $config->globals["rss_feed_loop/current_post"] =& $row;
    $config->globals["rss_feed_loop/current_item"] =& $item;
    $this_module->load_extensions("rss_feed_generator", "before_loop_iteration_ending");
    
    $tmp = @json_encode($item->title);       if( ! $tmp ) $item->title       = utf8_encode($item->title);
    $tmp = @json_encode($item->description); if( ! $tmp ) $item->description = utf8_encode($item->description);
    $tmp = @json_encode($item->author);      if( ! $tmp ) $item->author      = utf8_encode($item->author);
    $tmp = @json_encode($item->category);    if( ! $tmp ) $item->category    = utf8_encode($item->category);
    
    $channel->items[] = $item;
}
