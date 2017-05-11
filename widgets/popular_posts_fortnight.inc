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
 * @var toolbox           $toolbox
 * 
 * @return string
 */

use hng2_base\module;
use hng2_base\template;
use hng2_modules\posts\posts_repository;
use hng2_modules\widgets_manager\toolbox;

global $config, $language, $mem_cache, $account;

if( empty($config->globals["widgets_manager:toolbox"]) )
    $config->globals["widgets_manager:toolbox"] = new toolbox();
$toolbox = $config->globals["widgets_manager:toolbox"];
$placed_widget = $toolbox->get_widget_from_data_key($data_key);
if( is_null($placed_widget) ) return "";
$data = $toolbox->load_widget_data($placed_widget);

$limit     = $data["limit"];         if( empty($limit)   ) $limit   = 10;
$version   = $data["cache_version"]; if( empty($version) ) $version = 1;
$cache_ttl = $data["cache_ttl"] * 3600;
$cache_key = "modules:posts.popular_fortnight_widget-v{$version}";

if( $cache_ttl > 0 )
{
    $contents = $mem_cache->get($cache_key);
    if( ! empty($contents) && $contents != "none" ) return $contents;
    if( $contents == "none" ) return "";
}

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

$repository = new posts_repository();
$where      = array("publishing_date >= '$min_date' and publishing_date <= '$max_date'");
$posts      = $repository->lookup($where, 10, 0, "views desc");

if( empty($posts) )
{
    if( $cache_ttl > 0 ) $mem_cache->set($cache_key, "none", 0, $cache_ttl);
    
    return "";
}

ob_start(); ?>

<!-- posts/popular-fortnight  -->

<div class="posts_fortnight" data-from="<?= $min_date ?>" data-to="<?= $max_date ?>">
    <ol class="simple_entries_listing">
        <? foreach($posts as $post): ?>
            <li>
                <span class="pull-right"><?= number_format($post->views) ?></span>
                <a class="title" href="<?= $config->full_root_path ?>/<?= $post->id_post ?>"><?= $post->get_processed_title(false) ?></a><br>
                <span class="date"><?= time_elapsed_string($post->publishing_date) ?></span>
                <?= $post->main_category_title ?><br>
                <span class="author user_display_name" data-user-level="<?= $post->author_level ?>">
                    <span class="fa fa-user"></span>
                    <?= $post->get_processed_author_display_name() ?></span>
            </li>
        <? endforeach; ?>
    </ol>
</div>

<?
$return = ob_get_clean();
if( $cache_ttl > 0 ) $mem_cache->set($cache_key, $return, 0, $cache_ttl);

return $return;
