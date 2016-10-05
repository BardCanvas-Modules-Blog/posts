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
 * @return string
 */

use hng2_base\module;
use hng2_base\template;
use hng2_modules\posts\posts_repository;

global $config, $language, $mem_cache;

# TODO: Add data extractors!!!
$tag     = "polemicas";
$limit   = 10;
$order   = "publishing_date asc";
$version = 8;

$contents = $mem_cache->get("{$data_key}_v{$version}");
if( ! empty($contents) && $contents != "none" ) return $contents;
if( $contents == "none" ) return "";

$repository = new posts_repository();
$where      = array("id_post in (select id_post from post_tags where tag = '$tag')");
$posts      = $repository->lookup($where, $limit, 0, $order);

if( empty($posts) )
{
    $mem_cache->set("{$data_key}_v{$version}", "none", 0, 60*5);
    
    return "";
}

ob_start(); ?>

<!-- posts/posts_by_tag  -->

<div class="posts_by_tag" data-tag="<?= $tag ?>">
    <ul class="simple_entries_listing">
        <? foreach($posts as $post): ?>
            <li>
                <a class="author user_display_name" data-user-level="<?= $post->author_level ?>" href="<?= $config->full_root_path ?>/author/<?= $post->author_user_name ?>">
                    <span class="fa fa-user"></span>
                    <?= $post->get_processed_author_display_name() ?></a>
                <span class="date"><?= time_elapsed_string($post->publishing_date) ?></span>
                <a class="title" href="<?= $config->full_root_path ?>/<?= $post->id_post ?>"><?= $post->get_processed_title(false) ?></a>
            </li>
        <? endforeach; ?>
    </ul>
</div>

<?
$return = ob_get_clean();
$mem_cache->set("{$data_key}_v{$version}", $return, 0, 60*5);

return $return;
