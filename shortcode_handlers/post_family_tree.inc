<?php
/**
 * Handler for post_family_tree shortcode
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
 * @var string   $matched_str
 * @var string   $contents
 * @var array    $attributes
 *
 * Imported globals:
 * @var post_record $post
 *
 * Expected attributes:
 * • title string|nothing
 * • class empty or css class list
 * • style css styles
 * • reach children|all defaults to all
 */

use hng2_base\account;
use hng2_base\config;
use hng2_base\module;
use hng2_base\settings;
use hng2_modules\posts\post_record;
use hng2_modules\posts\posts_repository;
use hng2_modules\posts\toolbox;

global $post;
if( ! is_object($post) )
{
    $contents = str_replace($matched_str, "", $contents);
    
    return;
}

$repository = new posts_repository();
if( empty($attributes["reach"]) ) $attributes["reach"] = "all";
if( $attributes["reach"] == "all" && empty($post->parent_post) ) $attributes["reach"] = "children";

$tree = array();
if( $attributes["reach"] == "children" )
{
    $post->children = $repository->find_child_posts($post->id_post);
    if( empty($post->children) )
    {
        $contents = str_replace($matched_str, "", $contents);
        return;
    }
    $tree[$post->id_post] = $post;
}
else # $attributes["reach"] == "all"
{
    # Let's find the topmost parent
    $topmost = $repository->get_topmost_parent($post->id_post);
    $topmost->children = $repository->find_child_posts($topmost->id_post);
    if( empty($topmost->children) )
    {
        $contents = str_replace($matched_str, "", $contents);
        
        return;
    }
    
    $tree[$topmost->id_post] = $topmost;
}

if( empty($tree) )
{
    $contents = str_replace($matched_str, "", $contents);
    
    return;
}

$toolbox = new toolbox();

$list = $toolbox->build_post_family_tree_links($tree, $post->id_post);
if( empty($list) )
{
    $contents = str_replace($matched_str, "", $contents);
    
    return;
}

$html  = "<div class='post_family_tree {$attributes["class"]}' data-post-id='{$post->id_post}' style='{$attributes["style"]}'>"
       . "<div class='title'><span class='expand_trigger pull-right'
               onclick=\"$(this).closest('.post_family_tree').toggleClass('expanded')\"><i 
               class='expand   fa fa-chevron-down fa-border'></i><i 
               class='collapse fa fa-chevron-up   fa-border'></i></span>"
       . ( empty($attributes["title"]) ? $this_module->language->post_tree_index_caption : $attributes["title"] )
       . "</div>"
       . $list
       . "</div>";

$contents = str_replace($matched_str, $html, $contents);
