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
 * @return string
 */

use hng2_base\module;
use hng2_base\template;
use hng2_modules\posts\post_record;
use hng2_modules\posts\posts_repository;

global $config, $language, $mem_cache;

/** @var post_record $post */
$post = $template->get("current_post");
if( ! is_object($post) ) return "";

$data_key .= "/{$post->main_category_slug}/v6";

# TODO: Add data extractors!!!
$category_id = $post->main_category;
$limit       = 5;
$order       = "publishing_date desc";

$posts = $mem_cache->get($data_key);
if( is_array($posts) && empty($posts) ) return "";
if( $posts == "none" ) return "";

if( empty($posts) )
{
    $repository = new posts_repository();
    $where      = array("main_category = '$category_id'", "id_post <> '{$post->id_post}'");
    $posts      = $repository->lookup($where, $limit, 0, $order);
}

if( empty($posts ) )
{
    $mem_cache->set($data_key, "none", 0, 60*5);
    
    return "";
}

$mem_cache->set($data_key, $posts, 0, 60*5);

$posts = array_reverse($posts);
ob_start(); ?>

<!-- posts/other_posts_in_category  -->

<div class="other_posts_in_category" data-category-id="" data-category-slug="">
    <ul class="simple_entries_listing">
        <? foreach( $posts as $post ): ?>
            <li>
                <a class="author user_display_name" data-user-level="<?= $post->author_level ?>"
                   href="<?= $config->full_root_path ?>/author/<?= $post->author_user_name ?>">
                    <span class="fa fa-user"></span>
                    <?= $post->get_processed_author_display_name() ?>
                </a>
                <span class="date"><?= time_elapsed_string($post->publishing_date) ?></span>
                <a class="title" href="<?= $config->full_root_path ?>/<?= $post->id_post ?>"><?= $post->get_processed_title(false) ?></a>
            </li>
        <? endforeach; ?>
    </ul>
</div>

<? return ob_get_clean();
