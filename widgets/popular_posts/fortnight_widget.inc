<?php
/**
 * Popular posts widget: fortnight (current half of month)
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

global $config, $language, $mem_cache, $account;

$version = 3;
$ttl     = 60*60;

if( date("d") <= 15 )
{
    $min_date = date("Y-m-01 00:00:00");
    $max_date = date("Y-m-15 23:59:59");
}
else
{
    $min_date = date("Y-m-16 00:00:00");
    $max_date = date("Y-m-d 23:59:59", strtotime("last day of this month"));
}

$contents = $mem_cache->get("{$data_key}_v{$version}");
if( ! empty($contents) && $contents != "none" ) return $contents;
if( $contents == "none" ) return "";

$repository = new posts_repository();
$where      = array("publishing_date >= '$min_date' and publishing_date <= '$max_date'");
$posts      = $repository->lookup($where, 10, 0, "views desc");

if( empty($posts) )
{
    $mem_cache->set("{$data_key}_v{$version}", "none", 0, $ttl);
    
    return "";
}

ob_start(); ?>

<!-- posts/popular-fortnight  -->

<div class="posts_fortnight" data-from="<?= $min_date ?>" data-to="<?= $max_date ?>">
    <ol class="simple_entries_listing">
        <? foreach($posts as $post): ?>
            <li>
                <span class="pull-right"><?= number_format($post->views) ?></span>
                <a class="author user_display_name" data-user-level="<?= $post->author_level ?>" href="<?= $config->full_root_path ?>/author/<?= $post->author_user_name ?>">
                    <span class="fa fa-user"></span>
                    <?= $post->get_processed_author_display_name() ?></a>
                <span class="date"><?= time_elapsed_string($post->publishing_date) ?></span>
                <a class="title" href="<?= $config->full_root_path ?>/<?= $post->id_post ?>"><?= $post->get_processed_title(false) ?></a>
            </li>
        <? endforeach; ?>
    </ol>
</div>

<?
$return = ob_get_clean();
$mem_cache->set("{$data_key}_v{$version}", $return, 0, $ttl);

return $return;
