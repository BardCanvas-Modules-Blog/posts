<?php
/**
 * Frontend index of posts by category
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var config          $config
 * @var template        $template
 * @var category_record $category
 * @var module          $current_module
 */

use hng2_base\config;
use hng2_base\module;
use hng2_base\template;
use hng2_modules\categories\category_record;
use hng2_modules\posts\posts_repository;

$config->globals["modules:posts.by_category/post_index_renderer"]     = "{$template->abspath}/segments/post_index_renderer.inc";
$config->globals["modules:posts.by_category/abort_after_prefetching"] = false;
$current_module->load_extensions("posts_by_category_index", "pre_fetching");
if( $config->globals["modules:posts.by_category/abort_after_prefetching"] ) return;

$posts_repository      = new posts_repository();
$posts_data            = $posts_repository->get_for_category($category->id_category, true);
$pagination_url_prefix = "$config->full_root_url/category/{$category->slug}/";
$template->set("pagination_url_prefix", $pagination_url_prefix);
$config->globals["modules:posts.by_category/posts_data"] = $posts_data;

$config->globals["modules:posts.by_category/abort_before_rendering"] = false;
$current_module->load_extensions("posts_by_category_index", "pre_rendering");
if( $config->globals["modules:posts.by_category/abort_before_rendering"] ) return;

$config->globals["modules:posts.by_category/prevent_showing_heading"] = false;
$current_module->load_extensions("posts_by_category_index", "before_heading");

if( ! $config->globals["modules:posts.by_category/prevent_showing_heading"] ): ?>
    <h1 class="clearfix">
        <span class="title"><?= replace_escaped_vars($current_module->language->pages->by_category->title, '{$category}', $category->title) ?></span>
        <? if($posts_data->pagination["total_pages"] > 1): ?>
            <span class="title_pagination pull-right"><?= $posts_data->pagination["this_page_number"]; ?>/<?= $posts_data->pagination["total_pages"]; ?></span>
        <? endif; ?>
    </h1>
    <?
endif;

$current_module->load_extensions("posts_by_category_index", "after_heading");

if($posts_data->count == 0): ?>
    <div class='framed_content state_ko'>
        <span class='fa fa-warning'><?= $current_module->language->pages->by_category->no_records ?></span>
    </div>
    <?
endif;

include "{$config->globals["modules:posts.by_category/post_index_renderer"]}"; ?>
