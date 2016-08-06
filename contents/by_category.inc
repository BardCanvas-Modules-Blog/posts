<?php
/**
 * Frontend index of posts by category
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var config $config
 * @var template $template
 * @var category_record $category
 */

use hng2_base\config;
use hng2_base\template;
use hng2_modules\categories\category_record;
use hng2_modules\posts\posts_repository;

$posts_repository      = new posts_repository();
$posts_data            = $posts_repository->get_for_category($category->id_category);
$pagination_url_prefix = "$config->full_root_url/category/{$category->slug}/";
$template->set("pagination_url_prefix", $pagination_url_prefix);
?>

<h1 class="clearfix">
    <?= replace_escaped_vars($current_module->language->pages->by_category->title, '{$category}', $category->title) ?>
    <span class="pull-right"><?= $posts_data->pagination["this_page_number"]; ?>/<?= $posts_data->pagination["total_pages"]; ?></span>
</h1>

<? if($posts_data->count == 0): ?>
    <div class='framed_content state_ko'>
        <span class='fa fa-warning'><?= $current_module->language->pages->by_category->no_records ?></span>
    </div>
<? endif; ?>

<div id="quick_post_form_container">
    <div align="center" style="margin: 10px; color: maroon;">
        This is the quick post form
    </div>
</div>

<? if($posts_data->count == 0) return; ?>

<? $template->set("show_quick_post_form", false); ?>
<? include "{$template->abspath}/segments/post_index_renderer.inc"; ?>
