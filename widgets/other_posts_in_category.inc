<?php
/**
 * Other posts in the same category widget for right sidebar
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
use hng2_modules\posts\post_record;
use hng2_modules\posts\posts_repository;
use hng2_modules\widgets_manager\toolbox;

global $config, $language, $mem_cache;

if( empty($config->globals["widgets_manager:toolbox"]) )
    $config->globals["widgets_manager:toolbox"] = new toolbox();
$toolbox = $config->globals["widgets_manager:toolbox"];
$placed_widget = $toolbox->get_widget_from_data_key($data_key);
if( is_null($placed_widget) ) return "";
$data = $toolbox->load_widget_data($placed_widget);

/** @var post_record $post */
$post = $template->get("current_post");
if( ! is_object($post) ) return "";

$cache_key   = "modules:posts.other_in_category:{$post->main_category_slug}/v{$data["cache_version"]}";
$category_id = $post->main_category;
$limit       = $data["limit"]; if( empty($limit) ) $limit = 5;
$order       = $data["order"]; if( empty($order) ) $order = "desc";
$cache_ttl   = $data["cache_ttl"]; if ( empty($cache_ttl) ) $cache_ttl = 0;
$cache_ttl   = $cache_ttl * 60;
$posts       = array();

if( ! empty($cache_ttl) )
{
    $posts = $mem_cache->get($cache_key);
    if( is_array($posts) && empty($posts) ) return "";
    if( $posts == "none" ) return "";
}

if( empty($posts) )
{
    $repository = new posts_repository();
    $where      = array("main_category = '$category_id'", "id_post <> '{$post->id_post}'");
    $posts      = $repository->lookup($where, $limit, 0, "publishing_date desc");
}

if( empty($posts ) )
{
    if( ! empty($cache_ttl) ) $mem_cache->set($cache_key, "none", 0, $cache_ttl);
    
    return "";
}

if( ! empty($cache_ttl) ) $mem_cache->set($cache_key, $posts, 0, $cache_ttl);

if($order == "desc") $posts = array_reverse($posts);
ob_start(); ?>

<!-- posts/other_posts_in_category  -->

<div class="other_posts_in_category" data-category-id="" data-category-slug="">
    <ul class="simple_entries_listing">
        <? foreach( $posts as $post ): ?>
            <li>
                <a class="title" href="<?= $config->full_root_path ?>/<?= $post->id_post ?>"><?= $post->get_processed_title(false) ?></a><br>
                <span class="date"><?= time_elapsed_string($post->publishing_date) ?></span>
                <span class="author user_display_name" data-user-level="<?= $post->author_level ?>">
                    <span class="fa fa-user"></span>
                    <?= $post->get_processed_author_display_name() ?>
                </span>
            </li>
        <? endforeach; ?>
    </ul>
</div>

<? return ob_get_clean();
