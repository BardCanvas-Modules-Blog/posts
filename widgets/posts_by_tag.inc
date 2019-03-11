<?php
/**
 * Posts by tag widget for right sidebar
 * Called as include() from within a function
 *
 * @package    HNG2
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Trailing vars:
 * @var module[]          $modules
 * @var module            $this_module    widgets_manager
 * @var template          $template       The caller
 * @var \SimpleXMLElement $widget
 * @var string            $seed
 * @var string            $data_key
 * 
 * @var toolbox           $toolbox
 * 
 * @return string
 */

use hng2_base\module;
use hng2_base\template;
use hng2_modules\posts\posts_repository;
use hng2_modules\widgets_manager\toolbox;

global $config, $language, $mem_cache;

if( empty($config->globals["widgets_manager:toolbox"]) )
    $config->globals["widgets_manager:toolbox"] = new toolbox();
$toolbox = $config->globals["widgets_manager:toolbox"];
$placed_widget = $toolbox->get_widget_from_data_key($data_key);
if( is_null($placed_widget) ) return "";
$data = $toolbox->load_widget_data($placed_widget);
if( empty($data["tag"]) ) return "";

$tag       = str_replace("#", "", $data["tag"]);
$limit     = $data["limit"];         if( empty($limit)   ) $limit = 10;
$order     = $data["order"];         if( empty($order)   ) $order = "publishing_date asc";
$version   = $data["cache_version"]; if( empty($version) ) $version = 1;
$cache_ttl = $data["cache_ttl"] * 60;
$cache_key = "modules:posts.posts_by_tag_widget/$data_key-v{$version}";

if( $cache_ttl > 0 )
{
    $contents = $mem_cache->get($cache_key);
    if( ! empty($contents) && $contents != "none" ) return $contents;
    if( $contents == "none" ) return "";
}

$repository = new posts_repository();
$where      = array("id_post in (select id_post from post_tags where tag = '$tag')");
$posts      = $repository->lookup($where, $limit, 0, $order);

if( empty($posts) )
{
    if( $cache_ttl > 0 ) $mem_cache->set($cache_key, "none", 0, $cache_ttl);
    
    return "";
}

ob_start(); ?>

<!-- posts/posts_by_tag  -->

<div class="posts_by_tag" data-tag="<?= $tag ?>">
    <ul class="simple_entries_listing">
        <? foreach($posts as $post): ?>
            <li data-post-id="<?= $post->id_post ?>">
                <a class="title" href="<?= $post->get_permalink() ?>"><?= $post->get_processed_title(false) ?></a><br>
                <span class="date"><?= time_elapsed_string($post->publishing_date) ?></span>
                <?= $post->main_category_title ?><br>
                <span class="author user_display_name" data-user-level="<?= $post->author_level ?>">
                    <span class="fa fa-user"></span>
                    <?= $post->get_processed_author_display_name() ?></span>
            </li>
        <? endforeach; ?>
    </ul>
</div>

<?
$return = ob_get_clean();
if( $cache_ttl > 0 ) $mem_cache->set($cache_key, $return, 0, $cache_ttl);

return $return;
