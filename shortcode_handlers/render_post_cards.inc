<?php
/**
 * Handler for render_post_cards shortcode
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * Trailing globals:
 * @var module[] $modules
 * @var module   $current_module Topmost module
 * @var module   $this_module    Self
 * @var config   $config
 * @var settings $settings
 * @var account  $account
 *
 * Trailing vars:
 * @var array    $matches
 * @var string   $matched_str
 * @var string   $contents
 * @var array    $attributes
 *
 * Expected attributes:
 * • category     category slug, optional
 * • limit        10 by default
 * • order        latest|oldest, latests by default
 * • link_caption optional permalink caption. If empty, "View" will be rendered.
 * • left_icon    icon to show at the left of the permalink caption
 * • right_icon   icon to show at the left of the permalink caption
 * • class        empty or css class list
 * • style        empty or css styles
 */

use hng2_base\account;
use hng2_base\config;
use hng2_base\module;
use hng2_base\settings;
use hng2_modules\posts\posts_repository;

global $config, $settings;

$category     = trim($attributes["category"]);
$limit        = (int) $attributes["limit"]; if( empty($limit) ) $limit = 10;
$order        = trim($attributes["order"]); if( empty($order) ) $order = "latest";
$left_icon    = trim($attributes["left_icon"]);
$right_icon   = trim($attributes["right_icon"]);
$link_caption = trim($attributes["link_caption"]);
if( empty($link_caption) ) $link_caption = trim($this_module->language->read_more);

if( empty($attributes["category"]) )
{
    $contents = str_replace($matched_str, "", $contents);
    
    return;
}

static $posts_repository = null;
if( is_null($posts_repository) ) $posts_repository = new posts_repository();

$date   = date("Y-m-d H:i:s");
$filter = array(
    "main_category in ( select c.id_category from categories c where c.slug = '$category' )",
    "visibility" => "public",
    "status" => "published",
    "publishing_date <= '$date'"
);
$sqlorder = $order == "oldest" ? "publishing_date asc" : "publishing_date desc";
$posts    = $posts_repository->find($filter, $limit, 0, $sqlorder);

if( empty($posts) )
{
    $contents = str_replace($matched_str, "", $contents);
    
    return;
}

$return = "";
foreach($posts as $post)
{
    $date_ago = time_elapsed_string($post->publishing_date);
    $mini_date = time_today_string($post->publishing_date);
    $image     = $post->featured_image_thumbnail;
    if( empty($image) ) $image = $settings->get("engine.meta_default_image");
    if( empty($image) ) $image = "{$config->full_root_path}/media/missing_image.png";
    
    $return .= "
        <card>
            <title>{$post->get_processed_title(false)}</title>
            <excerpt>{$post->get_processed_excerpt(true)}</excerpt>
            <thumbnail><img src='{$image}'></thumbnail>
            <date><i class='fa fa-calendar'></i> {$post->publishing_date}</date>
            <date_ago><i class='fa fa-calendar'></i> {$date_ago}</date_ago>
            <mini_date><i class='fa fa-calendar'></i> {$mini_date}</mini_date>
            <permalink><a href='{$post->get_permalink()}'>{$left_icon} $link_caption {$right_icon}</a></permalink>
        </card>
    ";
}

$return = "<div class='{$attributes["class"]}' style='{$attributes["style"]}'>$return</div>";
$contents = str_replace($matched_str, $return, $contents);
