<?php
/**
 * Handler for post_title shortcode
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
 * • id                           id of the post
 * • processed         true|false to show processed title or not, true by default
 * • include_autolinks true|false true by default
 * • class                        empty or css class list
 * • style                        empty or css styles
 */

use hng2_base\account;
use hng2_base\config;
use hng2_base\module;
use hng2_base\settings;
use hng2_modules\posts\posts_repository;

if( empty($attributes["id"]) )
{
    $contents = str_replace($matched_str, "", $contents);
    
    return;
}

global $object_cache;

$repository = $object_cache->get("posts_shortcode_handlers", "posts_repository");
if( empty($repository) )
{
    $repository = new posts_repository();
    $object_cache->set("posts_shortcode_handlers", "posts_repository", $repository);
}

$post = $object_cache->get("posts_shortcode_handlers", "post_contents:{$attributes["id"]}");
if( is_null($post) )
{
    $post = $repository->get($attributes["id"]);
    if( ! is_null($post) ) $object_cache->set("posts_shortcode_handlers", "post_contents:{$attributes["id"]}", $post);
}

if( is_null($post) )
{
    $contents = str_replace($matched_str, "", $contents);
    
    return;
}

if( $post->status != "published" ) return;

$return = $attributes["processed"] != "false" ? $post->get_processed_title($attributes["include_autolinks"] != "false") : $post->title;
$return = "<span class='{$attributes["class"]}' style='{$attributes["style"]}'>$return</span>";
$contents = str_replace($matched_str, $return, $contents);
